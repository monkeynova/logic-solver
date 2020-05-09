#include "puzzle/filtered_solution_permuter.h"

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "puzzle/active_set.h"
#include "puzzle/active_set_builder.h"

DEFINE_bool(puzzle_prune_class_iterator, true,
            "If specfied, class iterators will be pruned based on single "
            "class predicates that are present.");

DEFINE_bool(puzzle_prune_pair_class_iterators, true,
            "If specfied, class iterators will be pruned based on pair "
            "class predicates that are present.");

DEFINE_bool(puzzle_prune_pair_class_iterators_mode_pair, true,
            "If specified pairwise iterators will be pruned with contextual "
            "pruning (that is, pairwise iterators will store, for each value "
            "of one iterator, the appropriate active sets for the other "
            "iterator).");

DEFINE_bool(puzzle_prune_reorder_classes, true,
            "If true, class iteration will be re-ordered from the default "
            "based on effective scan rate.");

namespace puzzle {

FilteredSolutionPermuter::Advancer::Advancer(
    const FilteredSolutionPermuter* permuter)
    : permuter_(permuter),
      mutable_solution_(permuter == nullptr ? nullptr
                                            : permuter_->entry_descriptor_) {
  if (permuter_ == nullptr) return;

  current_ = mutable_solution_.TestableSolution();
  iterators_.resize(permuter_->class_permuters_.size());
  pair_selectivity_reduction_.resize(iterators_.size(), 1);
  for (auto& class_permuter : permuter_->class_permuters_) {
    iterators_[class_permuter.class_int()] = class_permuter.end();
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

bool FilteredSolutionPermuter::Advancer::FindNextValid(int class_position) {
  if (static_cast<unsigned int>(class_position) >=
      permuter_->class_permuters_.size()) {
    return true;
  }

  const ClassPermuter& class_permuter =
      permuter_->class_permuters_[class_position];
  int class_int = class_permuter.class_int();

  const std::vector<SolutionFilter>& solution_predicates =
      permuter_->class_predicates_[class_int];

  if (iterators_[class_int] == class_permuter.end()) {
    const ActiveSetBuilder* builder = permuter_->active_set_builder_.get();
    ActiveSet build = builder->active_set(class_int);
    if (FLAGS_puzzle_prune_pair_class_iterators_mode_pair) {
      double start_selectivity = build.Selectivity();
      for (int other_pos = 0; other_pos < class_position; ++other_pos) {
        const ClassPermuter& other_permuter =
            permuter_->class_permuters_[other_pos];
        int other_class = other_permuter.class_int();
        int other_val = iterators_[other_class].position();
        build.Intersect(
            builder->active_set_pair(other_class, other_val, class_int));
      }
      pair_selectivity_reduction_[class_int] =
          build.Selectivity() / start_selectivity;
    }
    iterators_[class_int] = class_permuter.begin(std::move(build));
  }

  for (; iterators_[class_int] != class_permuter.end();
       ++iterators_[class_int]) {
    mutable_solution_.SetClass(iterators_[class_int]);
    if (NotePositionForProfiler(class_position)) return false;
    if (AllMatch(solution_predicates, current_) && 
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
      [this](std::string* out, const ClassPermuter& permuter) {
        if (iterators_[permuter.class_int()] == permuter.end()) {
          absl::StrAppend(out, "<->");
        } else {
          double truncated = iterators_[permuter.class_int()].Completion();
          truncated = static_cast<int>(1000 * truncated) / 1000.0;
          absl::StrAppend(out, truncated);
          if (FLAGS_puzzle_prune_pair_class_iterators_mode_pair) {
            double truncated =
                pair_selectivity_reduction_[permuter.class_int()];
            truncated = static_cast<int>(1000 * truncated) / 1000.0;
            absl::StrAppend(out, " (", truncated, ")");
          }
        }
      });
}

bool FilteredSolutionPermuter::Advancer::NotePositionForProfiler(
    int class_position) {
  VLOG(3) << "FindNextValid(" << class_position << ") ("
          << IterationDebugString() << ")";

  if (permuter_->profiler_ == nullptr) return false;

  if (permuter_->profiler_->NotePosition(position(),
                                         permuter_->permutation_count())) {
    std::cout << "; FindNextValid(" << class_position << ") ("
              << IterationDebugString() << ")" << std::flush;
  }

  return permuter_->profiler_->Done();
}

void FilteredSolutionPermuter::Advancer::Advance() {
  bool at_end = true;
  for (auto it = permuter_->class_permuters_.rbegin();
       it != permuter_->class_permuters_.rend(); ++it) {
    int class_int = it->class_int();

    if (iterators_[class_int] == it->end()) {
      iterators_[class_int] = it->begin();
    }

    ++iterators_[class_int];
    mutable_solution_.SetClass(iterators_[class_int]);

    bool carry = false;
    if (iterators_[class_int] == it->end()) {
      carry = true;
    }

    if (!carry) {
      at_end = false;
      break;
    }
  }
  if (!at_end && FindNextValid(0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

double FilteredSolutionPermuter::Advancer::position() const {
  double position = 0;

  for (auto& class_permuter : permuter_->class_permuters_) {
    position *= class_permuter.permutation_count();
    position += iterators_[class_permuter.class_int()].position();
  }

  return position;
}

double FilteredSolutionPermuter::Advancer::completion() const {
  return position() / permuter_->permutation_count();
}

FilteredSolutionPermuter::FilteredSolutionPermuter(const EntryDescriptor* e,
                                                   Profiler* profiler)
    : entry_descriptor_(e), profiler_(profiler) {}

bool FilteredSolutionPermuter::AddFilter(SolutionFilter solution_filter) {
  CHECK(!prepared_);
  // TODO(keith@monkeynova.com): Maybe test for full sized list as well.
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
  active_set_builder_ = absl::make_unique<ActiveSetBuilder>(entry_descriptor_);

  std::vector<int> class_order = entry_descriptor_->AllClasses()->Values();

  for (int class_int : class_order) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_.push_back(ClassPermuter(class_descriptor, class_int));
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
      int class_int = it->class_int();

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

  if (VLOG_IS_ON(1)) {
    for (const auto& permuter : class_permuters_) {
      const std::string predicate_name =
          class_predicates_[permuter.class_int()].empty()
              ? "<noop>"
              : absl::StrJoin(
                    class_predicates_[permuter.class_int()], "; ",
                    [](std::string* out, const SolutionFilter& filter) {
                      absl::StrAppend(out, filter.name());
                    });
      VLOG(1) << "Predicates at " << permuter.class_int() << " ("
              << permuter.Selectivity() << "="
              << permuter.Selectivity() * permuter.permutation_count()
              << "): " << class_predicates_[permuter.class_int()].size() << ": "
              << predicate_name;
    }
  }
}

void FilteredSolutionPermuter::BuildActiveSets(
    std::vector<SolutionFilter>* residual) {
  if (!FLAGS_puzzle_prune_class_iterator) {
    for (const auto& filter : predicates_) {
      residual->push_back(filter);
    }
    return;
  }

  VLOG(1) << "Generating singleton selectivities";

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
      if (!FLAGS_puzzle_prune_pair_class_iterators_mode_pair) {
        residual->push_back(filter);
      }
    } else {
      residual->push_back(filter);
    }
  }

  for (auto& class_permuter : class_permuters_) {
    int class_int = class_permuter.class_int();
    active_set_builder_->Build(class_permuter,
                               single_class_predicates[class_int]);
    VLOG(2) << "Selectivity (" << class_permuter.class_int()
            << "): " << class_permuter.Selectivity() << " => "
            << active_set_builder_->active_set(class_int).Selectivity();
    class_permuter.set_active_set(active_set_builder_->active_set(class_int));
  }

  if (!FLAGS_puzzle_prune_pair_class_iterators) {
    return;
  }

  VLOG(1) << "Generating pair selectivities";

  bool cardinality_reduced = true;
  bool need_final = FLAGS_puzzle_prune_pair_class_iterators_mode_pair;
  ActiveSetBuilder::PairClassMode pair_class_mode =
      ActiveSetBuilder::PairClassMode::kSingleton;
  while (cardinality_reduced || need_final) {
    if (!cardinality_reduced) {
      // After cardinality settles, run one more time with make_pairs on the
      // smallest N^2 required.
      need_final = false;
      pair_class_mode = ActiveSetBuilder::PairClassMode::kMakePairs;
      VLOG(1) << "Running one more pass to generate pairs";
    }
    cardinality_reduced = false;

    ReorderEvaluation();

    for (auto it_a = class_permuters_.begin(); it_a != class_permuters_.end();
         ++it_a) {
      ClassPermuter& permuter_a = *it_a;
      for (auto it_b = it_a + 1; it_b != class_permuters_.end(); ++it_b) {
        ClassPermuter& permuter_b = *it_b;
        CHECK(permuter_a.class_int() != permuter_b.class_int());

        std::vector<SolutionFilter>& filters =
            pair_class_predicates[std::make_pair(permuter_a.class_int(),
                                                 permuter_b.class_int())];
        if (filters.empty()) continue;

        active_set_builder_->Build(*it_a, *it_b, filters, pair_class_mode);
        const ActiveSet& new_a =
            active_set_builder_->active_set(permuter_a.class_int());
        const ActiveSet& new_b =
            active_set_builder_->active_set(permuter_b.class_int());
        VLOG(2) << "Selectivity (" << permuter_a.class_int() << ", "
                << permuter_b.class_int() << "): (" << permuter_a.Selectivity()
                << ", " << permuter_b.Selectivity() << ") => ("
                << new_a.Selectivity() << ", " << new_b.Selectivity() << ")";
        if (permuter_a.Selectivity() > new_a.Selectivity()) {
          cardinality_reduced = true;
        }
        if (permuter_b.Selectivity() > new_b.Selectivity()) {
          cardinality_reduced = true;
        }
        permuter_a.set_active_set(new_a);
        permuter_b.set_active_set(new_b);
      }
    }

    if (FLAGS_puzzle_prune_pair_class_iterators_mode_pair) {
      CHECK(!(!need_final && cardinality_reduced));
    }
  }
}

void FilteredSolutionPermuter::ReorderEvaluation() {
  if (!FLAGS_puzzle_prune_reorder_classes) {
    return;
  }

  std::sort(class_permuters_.begin(), class_permuters_.end(),
            [](const ClassPermuter& a, const ClassPermuter& b) {
              return a.Selectivity() < b.Selectivity();
            });

  VLOG(1) << "Reordered to: "
          << absl::StrJoin(class_permuters_, ", ",
                           [](std::string* out, const ClassPermuter& a) {
                             absl::StrAppend(out, "(", a.class_int(), ",",
                                             a.Selectivity(), ")");
                           });
}

double FilteredSolutionPermuter::permutation_count() const {
  double count = 1;
  for (auto& permuter : class_permuters_) {
    count *= permuter.permutation_count();
  }
  return count;
}

}  // namespace puzzle
