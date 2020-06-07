#include "puzzle/filtered_solution_permuter.h"

#include "absl/flags/flag.h"
#include "glog/logging.h"
#include "puzzle/active_set.h"
#include "puzzle/all_match.h"
#include "puzzle/class_permuter_factory.h"
#include "puzzle/filter_to_active_set.h"

ABSL_FLAG(bool, puzzle_prune_class_iterator, true,
          "If specfied, class iterators will be pruned based on single "
          "class predicates that are present.");

ABSL_FLAG(bool, puzzle_prune_reorder_classes, true,
          "If true, class iteration will be re-ordered from the default "
          "based on effective scan rate.");

ABSL_FLAG(bool, puzzle_prune_pair_class_iterators, true,
          "If specfied, class iterators will be pruned based on pair "
          "class predicates that are present.");

ABSL_FLAG(bool, puzzle_prune_pair_class_iterators_mode_pair, false,
          "If specified pairwise iterators will be pruned with contextual "
          "pruning (that is, pairwise iterators will store, for each value "
          "of one iterator, the appropriate active sets for the other "
          "iterator).");

ABSL_FLAG(bool, puzzle_pair_class_mode_make_pairs, false,
          "If true, pairwise iterator pruning will always run with a mode of "
          "kMakePairs.");

namespace puzzle {

static void OrderSolutionFiltersByEntryId(std::vector<SolutionFilter>* list) {
  std::sort(list->begin(), list->end(),
            [](const SolutionFilter& a, const SolutionFilter& b) {
              return a.entry_id(-1) < b.entry_id(-1);
            });
}

FilteredSolutionPermuter::Advancer::Advancer(
    const FilteredSolutionPermuter* permuter)
    : AdvancerBase(permuter == nullptr ? nullptr : permuter->entry_descriptor_),
      permuter_(permuter) {
  if (permuter_ == nullptr) return;

  iterators_.resize(permuter_->class_permuters_.size());
  pair_selectivity_reduction_.resize(iterators_.size(), 1);
  for (const auto& class_permuter : permuter_->class_permuters_) {
    iterators_[class_permuter->class_int()] = class_permuter->end();
  }

  if (FindNextValid(/*class_position=*/0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

void FilteredSolutionPermuter::Advancer::InitializeIterator(
    const ClassPermuter* class_permuter, int class_position) {
  const int class_int = class_permuter->class_int();
  const FilterToActiveSet* builder = permuter_->filter_to_active_set_.get();
  iterators_[class_int] =
      class_permuter->begin().WithActiveSet(builder->active_set(class_int));
  if (absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair)) {
    double start_selectivity = iterators_[class_int].Selectivity();
    for (int other_pos = 0; other_pos < class_position; ++other_pos) {
      const ClassPermuter* other_permuter =
          permuter_->class_permuters_[other_pos].get();
      int other_class = other_permuter->class_int();
      int other_val = iterators_[other_class].position();
      iterators_[class_int] = std::move(iterators_[class_int])
                                  .WithActiveSet(builder->active_set_pair(
                                      other_class, other_val, class_int));
    }
    pair_selectivity_reduction_[class_int] =
        iterators_[class_int].Selectivity() / start_selectivity;
  }
}

bool FilteredSolutionPermuter::Advancer::FindNextValid(int class_position) {
  if (static_cast<unsigned int>(class_position) >=
      permuter_->class_permuters_.size()) {
    return true;
  }

  const ClassPermuter* class_permuter =
      permuter_->class_permuters_[class_position].get();
  const int class_int = class_permuter->class_int();

  if (iterators_[class_int] == class_permuter->end()) {
    InitializeIterator(class_permuter, class_position);
  }

  const std::vector<SolutionFilter>& solution_predicates =
      permuter_->class_predicates_[class_int];
  ClassPermuter::iterator::ValueSkip value_skip = {.value_index =
                                                       Entry::kBadId};
  for (; iterators_[class_int] != class_permuter->end();
       iterators_[class_int] += value_skip) {
    mutable_solution_.SetClass(iterators_[class_int]);
    if (NotePositionForProfiler(class_position)) return false;
    if (AllMatch(solution_predicates, current_, class_int, &value_skip) &&
        FindNextValid(class_position + 1)) {
      return true;
    }
  }

  // Didn't find an entry in iteration. Return "no match".
  return false;
}

std::string FilteredSolutionPermuter::Advancer::IterationDebugString() const {
  return absl::StrJoin(
      permuter_->class_permuters_, ", ",
      [this](std::string* out, const std::unique_ptr<ClassPermuter>& permuter) {
        if (iterators_[permuter->class_int()] == permuter->end()) {
          absl::StrAppend(out, "<->");
        } else {
          double truncated = iterators_[permuter->class_int()].Completion();
          truncated = static_cast<int>(1000 * truncated) / 1000.0;
          absl::StrAppend(out, truncated);
          if (absl::GetFlag(
                  FLAGS_puzzle_prune_pair_class_iterators_mode_pair)) {
            double truncated =
                pair_selectivity_reduction_[permuter->class_int()];
            truncated = static_cast<int>(1000 * truncated) / 1000.0;
            absl::StrAppend(out, " (", truncated, ")");
          }
        }
      });
}

bool FilteredSolutionPermuter::Advancer::NotePositionForProfiler(
    int class_position) {
  VLOG(3) << "FindNextVali(d" << class_position << ") ("
          << IterationDebugString() << ")";

  if (permuter_->profiler_ == nullptr) return false;

  if (permuter_->profiler_->NotePermutation(position(),
                                            permuter_->permutation_count())) {
    std::cout << "; FindNextValid(" << class_position << ") ("
              << IterationDebugString() << ")" << std::flush;
  }

  return permuter_->profiler_->Done();
}

void FilteredSolutionPermuter::Advancer::Advance() {
  bool found_value = false;
  for (int class_position = permuter_->class_permuters_.size() - 1;
       class_position >= 0; --class_position) {
    const ClassPermuter* class_permuter =
        permuter_->class_permuters_[class_position].get();
    int class_int = class_permuter->class_int();

    if (iterators_[class_int] == class_permuter->end()) {
      InitializeIterator(class_permuter, class_position);
    } else {
      ++iterators_[class_int];
    }
    mutable_solution_.SetClass(iterators_[class_int]);

    if (iterators_[class_int] != class_permuter->end()) {
      found_value = true;
      break;
    }
  }
  if (found_value && FindNextValid(0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

int64_t FilteredSolutionPermuter::Advancer::position() const {
  int64_t position = 0;

  for (auto& class_permuter : permuter_->class_permuters_) {
    position *= class_permuter->permutation_count();
    position += iterators_[class_permuter->class_int()].position();
  }

  return position;
}

double FilteredSolutionPermuter::Advancer::completion() const {
  return 1.0 * position() / permuter_->permutation_count();
}

FilteredSolutionPermuter::FilteredSolutionPermuter(const EntryDescriptor* e,
                                                   Profiler* profiler)
    : entry_descriptor_(e), profiler_(profiler) {}

bool FilteredSolutionPermuter::AddFilter(SolutionFilter solution_filter) {
  CHECK(!prepared_);
  // TODO(@monkeynova): Maybe test for full sized list as well.
  if (solution_filter.classes().empty()) {
    // No reason to store the predicate here as we require a full solution to
    // evaluate the predicate.
    return false;
  }
  predicates_.emplace_back(std::move(solution_filter));
  // If `predicate` is successfully stored, this class guarantees to honor it
  // in the returned solutions.
  return true;
}

void FilteredSolutionPermuter::Prepare() {
  CHECK(!prepared_);
  prepared_ = true;
  filter_to_active_set_ =
      absl::make_unique<FilterToActiveSet>(entry_descriptor_, profiler_);

  std::vector<int> class_order = entry_descriptor_->AllClasses()->Values();

  for (int class_int : class_order) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_.emplace_back(
        MakeClassPermuter(class_descriptor, class_int));
  }

  std::vector<SolutionFilter> unhandled;
  BuildActiveSets(&unhandled);
  ReorderEvaluation();

  class_predicates_.clear();
  class_predicates_.resize(class_permuters_.size());
  for (const auto& filter : unhandled) {
    bool added = false;
    for (auto it = class_permuters_.rbegin(); it != class_permuters_.rend();
         ++it) {
      int class_int = (*it)->class_int();

      auto it2 = std::find(filter.classes().begin(), filter.classes().end(),
                           class_int);

      if (it2 != filter.classes().end()) {
        class_predicates_[class_int].push_back(filter);
        added = true;
        break;  // class_int
      }
    }
    CHECK(added) << "Could not add filter for " << filter.name() << " ["
                 << absl::StrJoin(filter.classes(), ",") << "]";
  }

  for (auto& predicates : class_predicates_) {
    OrderSolutionFiltersByEntryId(&predicates);
  }

  if (VLOG_IS_ON(1)) {
    for (const auto& permuter : class_permuters_) {
      double selectivity =
          filter_to_active_set_->active_set(permuter->class_int())
              .Selectivity();
      const std::string predicate_name =
          class_predicates_[permuter->class_int()].empty()
              ? "<noop>"
              : absl::StrJoin(
                    class_predicates_[permuter->class_int()], "; ",
                    [](std::string* out, const SolutionFilter& filter) {
                      absl::StrAppend(out, filter.name());
                    });
      VLOG(1) << "Predicates at " << permuter->class_int() << " ("
              << selectivity << "="
              << selectivity * permuter->permutation_count()
              << "): " << class_predicates_[permuter->class_int()].size()
              << ": " << predicate_name;
    }
  }
}

void FilteredSolutionPermuter::BuildActiveSets(
    std::vector<SolutionFilter>* residual) {
  if (!absl::GetFlag(FLAGS_puzzle_prune_class_iterator)) {
    for (const auto& filter : predicates_) {
      residual->push_back(filter);
    }
    return;
  }

  std::vector<std::vector<SolutionFilter>> single_class_predicates;
  absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>>
      pair_class_predicates;
  single_class_predicates.resize(class_permuters_.size());
  for (const auto& filter : predicates_) {
    if (filter.classes().size() == 1) {
      int class_int = filter.classes()[0];
      single_class_predicates[class_int].push_back(filter);
    } else if (filter.classes().size() == 2) {
      std::pair<int, int> key1 =
          std::make_pair(filter.classes()[0], filter.classes()[1]);
      std::pair<int, int> key2 =
          std::make_pair(filter.classes()[1], filter.classes()[0]);
      pair_class_predicates[key1].push_back(filter);
      pair_class_predicates[key2].push_back(filter);
      if (!absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators) ||
          !absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair)) {
        residual->push_back(filter);
      }
    } else {
      residual->push_back(filter);
    }
  }

  for (auto& single_class_predicate_list : single_class_predicates) {
    OrderSolutionFiltersByEntryId(&single_class_predicate_list);
  }

  for (auto& pair_and_predicates : pair_class_predicates) {
    OrderSolutionFiltersByEntryId(&pair_and_predicates.second);
  }

  VLOG(1) << "Generating singleton selectivities";

  for (auto& class_permuter : class_permuters_) {
    int class_int = class_permuter->class_int();
    double old_selectivity =
        filter_to_active_set_->active_set(class_int).Selectivity();
    filter_to_active_set_->Build(class_permuter.get(),
                                 single_class_predicates[class_int]);
    VLOG(2) << "Selectivity (" << class_permuter->class_int()
            << "): " << old_selectivity << " => "
            << filter_to_active_set_->active_set(class_int).Selectivity();
  }

  if (!absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators)) {
    return;
  }

  VLOG(1) << "Generating pair selectivities";

  FilterToActiveSet::PairClassMode pair_class_mode =
      absl::GetFlag(FLAGS_puzzle_pair_class_mode_make_pairs)
          ? FilterToActiveSet::PairClassMode::kMakePairs
          : FilterToActiveSet::PairClassMode::kSingleton;

  struct ClassPair {
    ClassPair(ClassPermuter* a, ClassPermuter* b,
              const std::vector<SolutionFilter>* filters)
        : a(a), b(b), filters(filters), computed(false) {}

    void SetPairSelectivity(const FilterToActiveSet* filter_to_active_set) {
      double a_selectivity =
          filter_to_active_set->active_set(a->class_int()).Selectivity();
      double b_selectivity =
          filter_to_active_set->active_set(b->class_int()).Selectivity();
      if (a_selectivity > b_selectivity) {
        // Make `a` less selective than `b` for Build calls.
        std::swap(a, b);
      }
      pair_selectivity = a_selectivity * b_selectivity;
    }

    ClassPermuter* a;
    ClassPermuter* b;
    const std::vector<SolutionFilter>* filters;
    double pair_selectivity;
    bool computed;
  };
  struct ClassPairGreaterThan {
    bool operator()(const ClassPair& a, const ClassPair& b) const {
      if (a.computed ^ b.computed) {
        // Computed is "greater than" non-computed.
        return a.computed;
      }
      return a.pair_selectivity > b.pair_selectivity;
    }
  };
  std::vector<ClassPair> pairs;
  pairs.reserve(class_permuters_.size() * (class_permuters_.size() - 1) / 2);
  for (auto it_a = class_permuters_.begin(); it_a != class_permuters_.end();
       ++it_a) {
    for (auto it_b = it_a + 1; it_b != class_permuters_.end(); ++it_b) {
      CHECK((*it_a)->class_int() != (*it_b)->class_int());
      auto filters_it = pair_class_predicates.find(
          std::make_pair((*it_a)->class_int(), (*it_b)->class_int()));
      if (filters_it != pair_class_predicates.end() &&
          !filters_it->second.empty()) {
        pairs.emplace_back(it_a->get(), it_b->get(), &filters_it->second);
        pairs.back().SetPairSelectivity(filter_to_active_set_.get());
        VLOG(2) << "Initial Selectivity (" << pairs.back().a->class_int()
                << ", " << pairs.back().b->class_int()
                << "): " << pairs.back().pair_selectivity;
      }
    }
  }

  if (pairs.empty()) return;

  VLOG(1) << "Pruning pairs: " << pairs.size();

  std::make_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
  while (!pairs.begin()->computed) {
    std::pop_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    ClassPair& pair = pairs.back();

    double old_pair_selectivity = pair.pair_selectivity;
    filter_to_active_set_->Build(pair.a, pair.b, *pair.filters,
                                 pair_class_mode);
    pair.SetPairSelectivity(filter_to_active_set_.get());
    VLOG(2) << "Selectivity (" << pair.a->class_int() << ", "
            << pair.b->class_int() << "): " << old_pair_selectivity << " => "
            << pair.pair_selectivity;
    pair.computed = true;
    if (old_pair_selectivity > pair.pair_selectivity) {
      for (ClassPair& to_update : pairs) {
        if ((to_update.a == pair.a || to_update.b == pair.a) ^
            (to_update.a == pair.b || to_update.b == pair.b)) {
          to_update.computed = false;
          to_update.SetPairSelectivity(filter_to_active_set_.get());
        }
      }
      std::make_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    } else {
      CHECK(old_pair_selectivity == pair.pair_selectivity);
      std::push_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    }
  }

  if (absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair) &&
      pair_class_mode != FilterToActiveSet::PairClassMode::kMakePairs) {
    VLOG(1) << "Running one more pass to generate pairs";
    for (ClassPair& pair : pairs) {
      filter_to_active_set_->Build(
          pair.a, pair.b, *pair.filters,
          FilterToActiveSet::PairClassMode::kMakePairs);
    }
  }
}

void FilteredSolutionPermuter::ReorderEvaluation() {
  if (!absl::GetFlag(FLAGS_puzzle_prune_reorder_classes)) {
    return;
  }

  std::sort(
      class_permuters_.begin(), class_permuters_.end(),
      [this](const std::unique_ptr<ClassPermuter>& a,
             const std::unique_ptr<ClassPermuter>& b) {
        double a_selectivity =
            filter_to_active_set_->active_set(a->class_int()).Selectivity();
        double b_selectivity =
            filter_to_active_set_->active_set(b->class_int()).Selectivity();
        return a_selectivity < b_selectivity;
      });

  VLOG(1) << "Reordered to: "
          << absl::StrJoin(class_permuters_, ", ",
                           [this](std::string* out,
                                  const std::unique_ptr<ClassPermuter>& a) {
                             absl::StrAppend(out, "(", a->class_int(), ",",
                                             filter_to_active_set_
                                                 ->active_set(a->class_int())
                                                 .Selectivity(),
                                             ")");
                           });
}

int64_t FilteredSolutionPermuter::permutation_count() const {
  int64_t count = 1;
  for (const auto& permuter : class_permuters_) {
    count *= permuter->permutation_count();
  }
  return count;
}

}  // namespace puzzle
