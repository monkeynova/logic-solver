#ifndef PUZZLE_FILTERED_SOLUTION_PERMUTER_H
#define PUZZLE_FILTERED_SOLUTION_PERMUTER_H

#include "absl/memory/memory.h"
#include "puzzle/active_set_builder.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

class FilteredSolutionPermuter final : public SolutionPermuter {
 public:
  class Advancer final : public SolutionPermuter::AdvanceInterface {
   public:
    explicit Advancer(const FilteredSolutionPermuter* permuter);

    Advancer(const Advancer&) = delete;
    Advancer& operator=(const Advancer&) = delete;

    double position() const override;
    double completion() const override;

   private:
    const Solution& current() const override { return current_; }
    void Advance() override;

    void PruneClass(int class_int,
                    const std::vector<SolutionFilter>& predicates);
    bool FindNextValid(int class_position);

    std::string IterationDebugString() const;

    // If permuter_->profiler is not null, calls NotePosition and flushes a
    // status prompt to std::out. Returns true if the profiler signals an
    // early abort is indicated.
    bool NotePositionForProfiler(int class_position);

    const FilteredSolutionPermuter* permuter_ = nullptr;
    MutableSolution mutable_solution_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;  // Bound to mutable_solution_.
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
  iterator end() const override {
    return iterator(absl::make_unique<Advancer>(nullptr));
  }

  double permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const {
    return class_permuters_[class_int];
  }

  bool AddPredicate(absl::string_view name, Solution::Predicate predicate,
                    std::vector<int> class_int_restrict_list) override;

  void Prepare() override;

 private:
  // Builds ActiveSet for each element in 'class_permuters_' (if flag enabled).
  // Elements in 'filters' that are not completely evaluated by these active
  // sets are returned in 'residual'.
  void BuildActiveSets(std::vector<SolutionFilter>* residual);

  // Reorders 'class_permuters_' by increasing selectivity. The effect of this
  // is to mean that any filter evaluated on a partial set of 'class_permuters_'
  // maximally prunes unnecessary iteration.
  void ReorderEvaluation();

  const EntryDescriptor* entry_descriptor_ = nullptr;

  std::vector<SolutionFilter> predicates_;

  Profiler* profiler_;

  bool prepared_ = false;

  // Ordered by the evaluation order that is configured for 'class_predicates_'.
  // That is, if the first N permuters have been updated then permuting entries
  // N+1 and further should only be performed if
  // 'class_predicates_[class_permuter_[N-1].class_int()]' is true.
  std::vector<ClassPermuter> class_permuters_;

  // class_predicates_[class_int] is an array of predicate residuals to evaluate
  // after filling class_int.
  std::vector<std::vector<SolutionFilter>> class_predicates_;

  std::unique_ptr<ActiveSetBuilder> active_set_builder_;

  friend Advancer;
};

}  // namespace puzzle

#endif  // PUZZLE_FILTERED_SOLUTION_PERMUTER_H
