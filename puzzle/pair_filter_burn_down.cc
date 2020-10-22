#include "puzzle/pair_filter_burn_down.h"

#include "absl/flags/flag.h"
#include "absl/synchronization/mutex.h"
#include "glog/logging.h"
#include "puzzle/class_pair_selectivity.h"
#include "puzzle/solution_filter.h"
#include "thread/future.h"

ABSL_FLAG(bool, puzzle_pair_class_mode_make_pairs, false,
          "If true, pairwise iterator pruning will always run with a mode of "
          "kMakePairs.");

extern absl::Flag<bool> FLAGS_puzzle_prune_pair_class_iterators_mode_pair;

namespace puzzle {

absl::Status PairFilterBurnDown::BurnDown(
    absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>> pair_class_predicates) {
  for (auto& pair_and_predicates : pair_class_predicates) {
    int first_class_int = pair_and_predicates.first.first;
    std::sort(pair_and_predicates.second.begin(),
              pair_and_predicates.second.end(),
              SolutionFilter::LtByEntryId(first_class_int));
  }

  FilterToActiveSet::PairClassMode pair_class_mode =
      absl::GetFlag(FLAGS_puzzle_pair_class_mode_make_pairs)
          ? FilterToActiveSet::PairClassMode::kMakePairs
          : FilterToActiveSet::PairClassMode::kSingleton;

  std::vector<ClassPairSelectivity> pairs;
  pairs.reserve(class_permuters_.size() * (class_permuters_.size() - 1) / 2);
  for (auto it_a = class_permuters_.begin(); it_a != class_permuters_.end();
       ++it_a) {
    for (auto it_b = it_a + 1; it_b != class_permuters_.end(); ++it_b) {
      if ((*it_a)->class_int() == (*it_b)->class_int()) {
        return absl::InternalError("same class on pair");
      }
      auto filters_by_a_it = pair_class_predicates.find(
          std::make_pair((*it_a)->class_int(), (*it_b)->class_int()));
      if (filters_by_a_it != pair_class_predicates.end() &&
          !filters_by_a_it->second.empty()) {
        auto filters_by_b_it = pair_class_predicates.find(
            std::make_pair((*it_b)->class_int(), (*it_a)->class_int()));
        if (filters_by_b_it == pair_class_predicates.end()) {
          return absl::InternalError("Could not find filters");
        }
        pairs.emplace_back(it_a->get(), it_b->get(), &filters_by_a_it->second,
                           &filters_by_b_it->second);
        pairs.back().SetPairSelectivity(filter_to_active_set_);
        VLOG(2) << "Initial Selectivity (" << pairs.back().a()->class_int()
                << ", " << pairs.back().b()->class_int()
                << "): " << pairs.back().pair_selectivity() << " ("
                << pairs.back().filters_by_a()->size() << " filters)";
      }
    }
  }

  if (pairs.empty()) return absl::OkStatus();

  VLOG(1) << "Pruning pairs: " << pairs.size();

  std::make_heap(pairs.begin(), pairs.end(), ClassPairSelectivityGreaterThan());
  while (!pairs.begin()->computed()) {
    std::pop_heap(pairs.begin(), pairs.end(),
                  ClassPairSelectivityGreaterThan());
    ClassPairSelectivity& pair = pairs.back();

    double old_pair_selectivity = pair.pair_selectivity();
    ::thread::Future<absl::Status> st;
    executor_->Schedule([&]() {
      absl::Status build_st =
          filter_to_active_set_->Build(pair.a(), pair.b(), *pair.filters_by_a(),
                                       *pair.filters_by_b(), pair_class_mode);
      if (!build_st.ok()) {
        st.Publish(build_st);
        return;
      }
      pair.set_computed_a(true);
      pair.set_computed_b(true);
      pair.SetPairSelectivity(filter_to_active_set_);
      st.Publish(absl::OkStatus());
    });
    if (!st->ok()) return *st;
    VLOG(2) << "Selectivity (" << pair.a()->class_int() << ", "
            << pair.b()->class_int() << "): "
            << static_cast<int>(old_pair_selectivity / pair.pair_selectivity())
            << "x: " << old_pair_selectivity << " => "
            << pair.pair_selectivity();
    if (old_pair_selectivity > pair.pair_selectivity()) {
      for (ClassPairSelectivity& to_update : pairs) {
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
      std::make_heap(pairs.begin(), pairs.end(),
                     ClassPairSelectivityGreaterThan());
    } else {
      if (old_pair_selectivity != pair.pair_selectivity()) {
        return absl::InternalError("Selectivity shouldn't increase");
      }
      std::push_heap(pairs.begin(), pairs.end(),
                     ClassPairSelectivityGreaterThan());
    }
  }

  if (absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair) &&
      pair_class_mode != FilterToActiveSet::PairClassMode::kMakePairs) {
    VLOG(1) << "Running one more pass to generate pairs";
    for (ClassPairSelectivity& pair : pairs) {
      absl::Status st = filter_to_active_set_->Build(
          pair.a(), pair.b(), *pair.filters_by_a(), *pair.filters_by_b(),
          FilterToActiveSet::PairClassMode::kMakePairs);
      if (!st.ok()) return st;
    }
  }

  return absl::OkStatus();
}

};
