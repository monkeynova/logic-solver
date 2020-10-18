#include "puzzle/filtered_solution_permuter.h"

#include "absl/flags/flag.h"
#include "absl/synchronization/notification.h"
#include "glog/logging.h"
#include "puzzle/active_set.h"
#include "puzzle/all_match.h"
#include "puzzle/class_permuter_factory.h"
#include "puzzle/filter_to_active_set.h"
#include "thread/inline_executor.h"
#include "thread/pool.h"

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

ABSL_FLAG(bool, puzzle_thread_pool_executor, false,
          "If true Solver::Prepare will try to use a thread pool to speed "
          "up the work by using more CPU cores.");

namespace puzzle {

FilteredSolutionPermuter::Advancer::Advancer(
    const FilteredSolutionPermuter* permuter)
    : AdvancerBase(permuter == nullptr ? nullptr
                                       : permuter->entry_descriptor()),
      permuter_(permuter) {
  if (permuter_ == nullptr) {
    set_done();
    return;
  }

  iterators_.resize(permuter_->class_permuters_.size());
  pair_selectivity_reduction_.resize(iterators_.size(), 1);
  for (const auto& class_permuter : permuter_->class_permuters_) {
    iterators_[class_permuter->class_int()] = class_permuter->end();
  }

  if (FindNextValid(/*class_position=*/0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    set_done();
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
  ClassPermuter::iterator::ValueSkip value_skip;
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
  VLOG(3) << "FindNextValid(" << class_position << ") ("
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
    set_done();
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

double FilteredSolutionPermuter::Advancer::position() const {
  if (permuter_ == nullptr) return -1;

  double position = 0;

  for (const auto& class_permuter : permuter_->class_permuters_) {
    position *= class_permuter->permutation_count();
    position += iterators_[class_permuter->class_int()].position();
  }

  return position;
}

double FilteredSolutionPermuter::Advancer::completion() const {
  if (permuter_ == nullptr) return 1;
  return 1.0 * position() / permuter_->permutation_count();
}

FilteredSolutionPermuter::FilteredSolutionPermuter(const EntryDescriptor* e,
                                                   Profiler* profiler)
    : SolutionPermuter(e),
      profiler_(profiler),
      executor_(absl::GetFlag(FLAGS_puzzle_thread_pool_executor)
                    ? static_cast<::thread::Executor*>(
                          new ::thread::Pool(/*num_workers=*/4))
                    : static_cast<::thread::Executor*>(
                          new ::thread::InlineExecutor())) {}

absl::StatusOr<bool> FilteredSolutionPermuter::AddFilter(
    SolutionFilter solution_filter) {
  if (prepared_) {
    return absl::FailedPreconditionError("Permuter already prepared");
  }
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

absl::Status FilteredSolutionPermuter::Prepare() {
  if (prepared_) {
    return absl::FailedPreconditionError("Already prepared");
  }
  prepared_ = true;
  filter_to_active_set_ =
      absl::make_unique<FilterToActiveSet>(entry_descriptor(), profiler_);

  for (int class_int = 0; class_int < entry_descriptor()->AllClasses()->size();
       ++class_int) {
    const Descriptor* class_descriptor =
        entry_descriptor()->AllClassValues(class_int);
    class_permuters_.emplace_back(
        MakeClassPermuter(class_descriptor, class_int));
  }

  std::vector<SolutionFilter> unhandled;
  if (absl::Status st = BuildActiveSets(&unhandled); !st.ok()) return st;
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
    if (!added) {
      return absl::InternalError(
          absl::StrCat("Could not add filter for ", filter.name(), " [",
                       absl::StrJoin(filter.classes(), ","), "]"));
    }
  }

  for (auto& predicates : class_predicates_) {
    std::sort(predicates.begin(), predicates.end(),
              SolutionFilter::LtByEntryId());
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
  return absl::OkStatus();
}

absl::Status FilteredSolutionPermuter::BuildActiveSets(
    std::vector<SolutionFilter>* residual) {
  if (!absl::GetFlag(FLAGS_puzzle_prune_class_iterator)) {
    for (const auto& filter : predicates_) {
      residual->push_back(filter);
    }
    return absl::OkStatus();
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

  VLOG(1) << "Generating singleton selectivities";

  for (auto& single_class_predicate_list : single_class_predicates) {
    std::sort(single_class_predicate_list.begin(),
              single_class_predicate_list.end(), SolutionFilter::LtByEntryId());
  }

  for (const auto& class_permuter : class_permuters_) {
    int class_int = class_permuter->class_int();
    double old_selectivity =
        filter_to_active_set_->active_set(class_int).Selectivity();
    absl::Status st = filter_to_active_set_->Build(
        class_permuter.get(), single_class_predicates[class_int]);
    if (!st.ok()) return st;
    VLOG(2) << "Selectivity (" << class_permuter->class_int()
            << "): " << old_selectivity << " => "
            << filter_to_active_set_->active_set(class_int).Selectivity();
  }

  if (!absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators)) {
    return absl::OkStatus();
  }

  VLOG(1) << "Generating pair selectivities";

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

  class ClassPair {
   public:
    ClassPair(ClassPermuter* a, ClassPermuter* b,
              const std::vector<SolutionFilter>* filters_by_a,
              const std::vector<SolutionFilter>* filters_by_b)
        : a_(a),
          b_(b),
          filters_by_a_(filters_by_a),
          filters_by_b_(filters_by_b),
          computed_a_(false),
          computed_b_(false) {}

    ClassPermuter* a() const { return a_; }
    ClassPermuter* b() const { return b_; }
    const std::vector<SolutionFilter>* filters_by_a() const {
      return filters_by_a_;
    }
    const std::vector<SolutionFilter>* filters_by_b() const {
      return filters_by_b_;
    }
    double pair_selectivity() const { return pair_selectivity_; }
    // TODO(@monkeynova): In theory computed should be the AND-ing of the two
    // parts. In practice, it causes a 15% regression in benchmarks on sudoku
    // since it redoes a bunch of redundant work. This method probably should
    // be reamed at the least, but understanding the model for when to compute
    // further would be better.
    bool computed() const { return computed_a_ || computed_b_; }
    void set_computed_a(bool computed) { computed_a_ = computed; }
    void set_computed_b(bool computed) { computed_b_ = computed; }

    void SetPairSelectivity(const FilterToActiveSet* filter_to_active_set) {
      double a_selectivity =
          filter_to_active_set->active_set(a_->class_int()).Selectivity();
      double b_selectivity =
          filter_to_active_set->active_set(b_->class_int()).Selectivity();
      if (a_selectivity > b_selectivity) {
        // Make `a` less selective than `b` for Build calls.
        // TODO(@monkeynova): Leave not-computed on "the right" and pass down
        // that information to FilterToActiveSet.
        std::swap(a_, b_);
        std::swap(filters_by_a_, filters_by_b_);
        std::swap(computed_a_, computed_b_);
      }
      pair_selectivity_ = a_selectivity * b_selectivity;
    }

    // TODO(@monkeynova): This metric currently is based on the worst-case
    // cost of computing the pair-wise active sets (N^2 cost). But this is
    // neither the expected cost of the computation (early exit), nor is
    // that even the ideal metric which is the ROI on future compute reduction.
    bool operator<(const ClassPair& other) const {
      if (computed() ^ other.computed()) {
        // Computed is "greater than" non-computed.
        return other.computed();
      }
      return pair_selectivity() < other.pair_selectivity();
    }

   private:
    ClassPermuter* a_;
    ClassPermuter* b_;
    const std::vector<SolutionFilter>* filters_by_a_;
    const std::vector<SolutionFilter>* filters_by_b_;
    double pair_selectivity_;
    bool computed_a_;
    bool computed_b_;
  };
  struct ClassPairGreaterThan {
    bool operator()(const ClassPair& a, const ClassPair& b) const {
      return b < a;
    }
  };
  std::vector<ClassPair> pairs;
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
        pairs.back().SetPairSelectivity(filter_to_active_set_.get());
        VLOG(2) << "Initial Selectivity (" << pairs.back().a()->class_int()
                << ", " << pairs.back().b()->class_int()
                << "): " << pairs.back().pair_selectivity() << " ("
                << pairs.back().filters_by_a()->size() << " filters)";
      }
    }
  }

  if (pairs.empty()) return absl::OkStatus();

  VLOG(1) << "Pruning pairs: " << pairs.size();

  std::make_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
  while (!pairs.begin()->computed()) {
    std::pop_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    ClassPair& pair = pairs.back();

    double old_pair_selectivity = pair.pair_selectivity();
    absl::Status st;
    absl::Notification wait;
    executor_->Schedule([&]() {
      st =
          filter_to_active_set_->Build(pair.a(), pair.b(), *pair.filters_by_a(),
                                       *pair.filters_by_b(), pair_class_mode);
      wait.Notify();
    });
    wait.WaitForNotification();
    if (!st.ok()) return st;
    pair.set_computed_a(true);
    pair.set_computed_b(true);
    pair.SetPairSelectivity(filter_to_active_set_.get());
    VLOG(2) << "Selectivity (" << pair.a()->class_int() << ", "
            << pair.b()->class_int() << "): "
            << static_cast<int>(old_pair_selectivity / pair.pair_selectivity())
            << "x: " << old_pair_selectivity << " => "
            << pair.pair_selectivity();
    if (old_pair_selectivity > pair.pair_selectivity()) {
      for (ClassPair& to_update : pairs) {
        if (to_update.a() == pair.a() || to_update.a() == pair.b()) {
          // Skip exact match.
          if (to_update.b() != pair.a() && to_update.b() != pair.b()) {
            to_update.set_computed_a(false);
            to_update.SetPairSelectivity(filter_to_active_set_.get());
          }
        } else if (to_update.b() == pair.a() || to_update.b() == pair.b()) {
          to_update.set_computed_b(false);
          to_update.SetPairSelectivity(filter_to_active_set_.get());
        }
      }
      std::make_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    } else {
      if (old_pair_selectivity != pair.pair_selectivity()) {
        return absl::InternalError("Selectivity shouldn't increase");
      }
      std::push_heap(pairs.begin(), pairs.end(), ClassPairGreaterThan());
    }
  }

  if (absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators_mode_pair) &&
      pair_class_mode != FilterToActiveSet::PairClassMode::kMakePairs) {
    VLOG(1) << "Running one more pass to generate pairs";
    for (ClassPair& pair : pairs) {
      absl::Status st = filter_to_active_set_->Build(
          pair.a(), pair.b(), *pair.filters_by_a(), *pair.filters_by_b(),
          FilterToActiveSet::PairClassMode::kMakePairs);
      if (!st.ok()) return st;
    }
  }
  return absl::OkStatus();
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

double FilteredSolutionPermuter::permutation_count() const {
  double count = 1;
  for (const auto& permuter : class_permuters_) {
    count *= permuter->permutation_count();
  }
  return count;
}

}  // namespace puzzle
