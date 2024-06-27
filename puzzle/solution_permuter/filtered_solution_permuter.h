#ifndef PUZZLE_SOLUTION_PERMUTER_FILTERED_SOLUTION_PERMUTER_H
#define PUZZLE_SOLUTION_PERMUTER_FILTERED_SOLUTION_PERMUTER_H

#include "absl/memory/memory.h"
#include "absl/status/statusor.h"
#include "puzzle/base/profiler.h"
#include "puzzle/base/solution.h"
#include "puzzle/base/solution_filter.h"
#include "puzzle/class_permuter/class_permuter.h"
#include "puzzle/solution_permuter/filter_to_active_set.h"
#include "puzzle/solution_permuter/mutable_solution.h"
#include "puzzle/solution_permuter/solution_permuter.h"
#include "thread/executor.h"

namespace puzzle {

class FilteredSolutionPermuter final : public SolutionPermuter {
 public:
  class Advancer final : public SolutionPermuter::AdvancerBase {
   public:
    explicit Advancer(const FilteredSolutionPermuter* permuter);

    Advancer(const Advancer&) = delete;
    Advancer& operator=(const Advancer&) = delete;

    Position position() const;

   private:
    void Advance() override;

    void PruneClass(int class_int,
                    const std::vector<SolutionFilter>& predicates);
    bool FindNextValid(int class_position);

    std::string IterationDebugString() const;

    // Initializes the iterator corresponding to `class_permuter` to begin
    // iteration, while applying the appropriate ActiveSet filters.
    // `class_position` specifies the position of `class_permuter` within
    // `permuter_->permuters_` to apply pairwise ActiveSet filters if use of
    // them is flag enabled.
    void InitializeIterator(const ClassPermuter* class_permuter,
                            int class_position);

    // If permuter_->profiler is not null, calls NotePosition and flushes a
    // status prompt to std::out. Returns true if the profiler signals an
    // early abort is indicated.
    bool NotePositionForProfiler(int class_position);

    const FilteredSolutionPermuter* permuter_ = nullptr;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    std::vector<double> pair_selectivity_reduction_;
  };

  FilteredSolutionPermuter(const EntryDescriptor* e, Profiler* profiler);
  ~FilteredSolutionPermuter() = default;

  // Movable, but not copyable.
  FilteredSolutionPermuter(const FilteredSolutionPermuter&) = delete;
  FilteredSolutionPermuter& operator=(const FilteredSolutionPermuter&) = delete;
  FilteredSolutionPermuter(FilteredSolutionPermuter&&) = default;
  FilteredSolutionPermuter& operator=(FilteredSolutionPermuter&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this));
  }

  double Selectivity() const override;
  double permutation_count() const;

  absl::StatusOr<bool> AddFilter(SolutionFilter solution_filter) override;

  absl::Status PrepareCheap() override;
  absl::Status PrepareFull() override;

 private:
  // Builds ActiveSet for each element in 'class_permuters_' (if flag enabled).
  // Elements in 'filters' that are not completely evaluated by these active
  // sets are returned in 'residual'.
  absl::Status BuildActiveSetsCheap(std::vector<SolutionFilter>* residual);
  absl::Status BuildActiveSetsFull();

  // Reorders 'class_permuters_' by increasing selectivity. The effect of this
  // is to mean that any filter evaluated on a partial set of 'class_permuters_'
  // maximally prunes unnecessary iteration.
  void ReorderEvaluation();

  std::vector<SolutionFilter> predicates_;

  Profiler* profiler_;

  enum class PrepareState {
    kUnprepared = 0,
    kCheap = 1,
    kFull = 2,
  };
  PrepareState prepare_state_ = PrepareState::kUnprepared;
  struct PrepareCheapState {
    std::vector<SolutionFilter> residual;
    absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>>
        pair_class_predicates;
  } prepare_cheap_state_;

  // Ordered by the evaluation order that is configured for 'class_predicates_'.
  // That is, if the first N permuters have been updated then permuting entries
  // N+1 and further should only be performed if
  // 'class_predicates_[class_permuter_[N-1].class_int()]' is true.
  std::vector<std::unique_ptr<ClassPermuter>> class_permuters_;

  // class_predicates_[class_int] is an array of predicate residuals to evaluate
  // after filling class_int.
  std::vector<std::vector<SolutionFilter>> class_predicates_;

  std::unique_ptr<FilterToActiveSet> filter_to_active_set_;

  std::unique_ptr<::thread::Executor> executor_;

  friend Advancer;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_PERMUTER_FILTERED_SOLUTION_PERMUTER_H
