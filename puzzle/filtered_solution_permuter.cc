#include "puzzle/filtered_solution_permuter.h"

#include "absl/flags/flag.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/synchronization/notification.h"
#include "puzzle/active_set.h"
#include "puzzle/all_match.h"
#include "puzzle/class_permuter_factory.h"
#include "puzzle/filter_to_active_set.h"
#include "puzzle/pair_filter_burn_down.h"
#include "vlog.h"
#include "thread/future.h"
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

ABSL_FLAG(bool, puzzle_thread_pool_executor, false,
          "If true Solver::Prepare will try to use a thread pool to speed "
          "up the work by using more CPU cores.");

ABSL_FLAG(int, puzzle_thread_pool_executor_threads, 4,
          "If --puzzle_thread_pool_executor is true, this specifies the size "
          "of the threadpool used (number of concurrent threads)");

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
      executor_(
          absl::GetFlag(FLAGS_puzzle_thread_pool_executor)
              ? static_cast<::thread::Executor*>(new ::thread::Pool(
                    absl::GetFlag(FLAGS_puzzle_thread_pool_executor_threads)))
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

  ::thread::FutureSet<absl::Status> work_set;
  for (const auto& class_permuter : class_permuters_) {
    executor_->ScheduleFuture(
        &work_set, [this, &class_permuter, &single_class_predicates]() {
          int class_int = class_permuter->class_int();
          double old_selectivity =
              filter_to_active_set_->active_set(class_int).Selectivity();
          absl::Status st = filter_to_active_set_->Build(
              class_permuter.get(), single_class_predicates[class_int]);
          if (!st.ok()) return st;
          VLOG(2) << "Selectivity (" << class_permuter->class_int()
                  << "): " << old_selectivity << " => "
                  << filter_to_active_set_->active_set(class_int).Selectivity();
          return absl::OkStatus();
        });
  }
  while (::thread::Future<absl::Status>* st = work_set.WaitForAny()) {
    if (!(*st)->ok()) return **st;
  }

  if (!absl::GetFlag(FLAGS_puzzle_prune_pair_class_iterators)) {
    return absl::OkStatus();
  }

  VLOG(1) << "Generating pair selectivities";

  PairFilterBurnDown burn_down(class_permuters_,
                               std::move(pair_class_predicates),
                               filter_to_active_set_.get(), executor_.get());

  if (absl::Status st = burn_down.BurnDown(); !st.ok()) return st;

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
