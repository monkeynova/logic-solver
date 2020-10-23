#include "puzzle/pair_filter_burn_down.h"

#include "absl/flags/flag.h"
#include "absl/synchronization/mutex.h"
#include "glog/logging.h"
#include "puzzle/solution_filter.h"
#include "thread/future.h"

ABSL_FLAG(bool, puzzle_pair_class_mode_make_pairs, false,
          "If true, pairwise iterator pruning will always run with a mode of "
          "kMakePairs.");

extern absl::Flag<bool> FLAGS_puzzle_prune_pair_class_iterators_mode_pair;

namespace puzzle {

absl::StatusOr<std::vector<ClassPairSelectivity>>
PairFilterBurnDown::BuildSelectivityPairs() {
  absl::flat_hash_map<int, ClassPermuter*> class_int_to_permuter;
  for (const auto& permuter : class_permuters_) {
    class_int_to_permuter[permuter->class_int()] = permuter.get();
  }

  std::vector<ClassPairSelectivity> pairs;
  pairs.reserve(class_permuters_.size() * (class_permuters_.size() - 1) / 2);
  for (auto& pair_and_predicates : pair_class_predicates_) {
    int first_class_int = pair_and_predicates.first.first;
    int second_class_int = pair_and_predicates.first.second;
    std::vector<SolutionFilter>& filters_by_first = pair_and_predicates.second;

    if (first_class_int == second_class_int) {
      return absl::InvalidArgumentError(
          absl::StrCat("same class on pair: ", first_class_int));
    }

    auto reverse_pair = std::make_pair(second_class_int, first_class_int);
    auto reverse_it = pair_class_predicates_.find(reverse_pair);
    if (reverse_it == pair_class_predicates_.end()) {
      return absl::InvalidArgumentError(
          absl::StrCat("mapping does not contain reverse pair for: (",
                       first_class_int, ", ", second_class_int, ")"));
    }
    std::vector<SolutionFilter>& filters_by_second = reverse_it->second;
    // filters_by_{first,second} should represent the same set of filters just
    // ordered differently. Validate that at least they have the same size.
    if (filters_by_first.size() != filters_by_second.size()) {
      return absl::InvalidArgumentError(
          absl::StrCat("different sized filters for: (", first_class_int, ", ",
                       second_class_int, ")"));
    }
    // No filters for this pair.
    if (filters_by_first.empty()) continue;
    // Since we process both pairs at the same time and they aren't equal,
    // only handle the case where first < second and leave the > path to be
    // handled in the reverse.
    if (first_class_int > second_class_int) continue;

    ClassPermuter* first_class = class_int_to_permuter[first_class_int];
    if (first_class == nullptr) {
      return absl::InvalidArgumentError(
          absl::StrCat("class ", first_class_int, " is not in permuters"));
    }
    ClassPermuter* second_class = class_int_to_permuter[second_class_int];
    if (second_class == nullptr) {
      return absl::InvalidArgumentError(
          absl::StrCat("class ", second_class_int, " is not in permuters"));
    }
    std::sort(filters_by_first.begin(), filters_by_first.end(),
              SolutionFilter::LtByEntryId(first_class_int));
    std::sort(filters_by_second.begin(), filters_by_second.end(),
              SolutionFilter::LtByEntryId(second_class_int));
    pairs.emplace_back(first_class, second_class, &filters_by_first,
                       &filters_by_second);
    pairs.back().SetPairSelectivity(filter_to_active_set_);
    VLOG(2) << "Initial Selectivity (" << pairs.back().a()->class_int() << ", "
            << pairs.back().b()->class_int()
            << "): " << pairs.back().pair_selectivity() << " ("
            << pairs.back().filters_by_a()->size() << " filters)";
  }

  return pairs;
}

absl::Status PairFilterBurnDown::BurnDown() {
  absl::StatusOr<std::vector<ClassPairSelectivity>> pairs =
      BuildSelectivityPairs();
  if (!pairs.ok()) return pairs.status();
  if (pairs->empty()) return absl::OkStatus();

  VLOG(1) << "Pruning pairs: " << pairs->size();

  FilterToActiveSet::PairClassMode pair_class_mode =
      absl::GetFlag(FLAGS_puzzle_pair_class_mode_make_pairs)
          ? FilterToActiveSet::PairClassMode::kMakePairs
          : FilterToActiveSet::PairClassMode::kSingleton;

  std::make_heap(pairs->begin(), pairs->end(),
                 ClassPairSelectivityGreaterThan());
  ::thread::FutureSet<absl::Status> work;
  while (!pairs->begin()->computed()) {
    std::pop_heap(pairs->begin(), pairs->end(),
                  ClassPairSelectivityGreaterThan());
    ClassPairSelectivity& pair = pairs->back();

    double old_pair_selectivity = pair.pair_selectivity();
    executor_->ScheduleFuture<absl::Status>(
        &work, [this, &pair, pair_class_mode]() {
          absl::Status build_st = filter_to_active_set_->Build(
              pair.a(), pair.b(), *pair.filters_by_a(), *pair.filters_by_b(),
              pair_class_mode);
          if (!build_st.ok()) {
            return build_st;
          }
          pair.set_computed_a(true);
          pair.set_computed_b(true);
          pair.SetPairSelectivity(filter_to_active_set_);
          return absl::OkStatus();
        });
    ::thread::Future<absl::Status>* st = work.WaitForAny();
    if (!(*st)->ok()) return **st;

    VLOG(2) << "Selectivity (" << pair.a()->class_int() << ", "
            << pair.b()->class_int() << "): "
            << static_cast<int>(old_pair_selectivity / pair.pair_selectivity())
            << "x: " << old_pair_selectivity << " => "
            << pair.pair_selectivity();
    if (old_pair_selectivity > pair.pair_selectivity()) {
      for (ClassPairSelectivity& to_update : *pairs) {
        if (to_update.a() == pair.a() || to_update.a() == pair.b()) {
          // Skip exact match.
          if (to_update.b() != pair.a() && to_update.b() != pair.b()) {
            to_update.set_computed_a(false);
            to_update.SetPairSelectivity(filter_to_active_set_);
          }
        } else if (to_update.b() == pair.a() || to_update.b() == pair.b()) {
          to_update.set_computed_b(false);
          to_update.SetPairSelectivity(filter_to_active_set_);
        }
      }
      std::make_heap(pairs->begin(), pairs->end(),
                     ClassPairSelectivityGreaterThan());
    } else {
      if (old_pair_selectivity != pair.pair_selectivity()) {
        return absl::InternalError("Selectivity shouldn't increase");
      }
      std::push_heap(pairs->begin(), pairs->end(),
                     ClassPairSelectivityGreaterThan());
    }
  }

  if (absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair) &&
      pair_class_mode != FilterToActiveSet::PairClassMode::kMakePairs) {
    VLOG(1) << "Running one more pass to generate pairs";
    ::thread::FutureSet<absl::Status> make_pairs_work;
    for (ClassPairSelectivity& pair : *pairs) {
      executor_->ScheduleFuture<absl::Status>(
          &make_pairs_work, [this, &pair]() {
            return filter_to_active_set_->Build(
                pair.a(), pair.b(), *pair.filters_by_a(), *pair.filters_by_b(),
                FilterToActiveSet::PairClassMode::kMakePairs);
          });
    }
    while (::thread::Future<absl::Status>* st = make_pairs_work.WaitForAny()) {
      if (!(*st)->ok()) return **st;
    }
  }

  return absl::OkStatus();
}

};  // namespace puzzle
