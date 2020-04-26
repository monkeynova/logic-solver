#ifndef __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
#define __PUZZLE_CROPPED_SOLUTION_PERMUTER_H

#include "absl/memory/memory.h"
#include "puzzle/active_set_builder.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

class CroppedSolutionPermuter final : public SolutionPermuter {
 public:
  class Advancer final : public SolutionPermuter::AdvanceInterface {
   public:
    explicit Advancer(const CroppedSolutionPermuter* permuter);

    Advancer(const Advancer&) = delete;
    Advancer& operator=(const Advancer&) = delete;

    double position() const override;
    double completion() const override;

  private:
    const Solution& current() const override { return current_; }
    void Advance() override;

    void PruneClass(int class_int,
                    const std::vector<Solution::Cropper>& predicates);
    bool FindNextValid(int class_position);

    std::string IterationDebugString() const;

    // If permuter_->profiler is not null, calls NotePosition and flushes a
    // status prompt to std::out. Returns true if the profiler signals an
    // early abort is indicated.
    bool NotePositionForProfiler(int class_position);

    const CroppedSolutionPermuter* permuter_ = nullptr;
    MutableSolution mutable_solution_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;  // Bound to mutable_solution_.
    std::vector<double> pair_selectivity_reduction_;
  };

  CroppedSolutionPermuter(
      const EntryDescriptor* e,
      Profiler* profiler);
  ~CroppedSolutionPermuter() = default;

  // Movable, but not copyable.
  CroppedSolutionPermuter(const CroppedSolutionPermuter&) = delete;
  CroppedSolutionPermuter& operator=(const CroppedSolutionPermuter&) = delete;
  CroppedSolutionPermuter(CroppedSolutionPermuter&&) = default;
  CroppedSolutionPermuter& operator=(CroppedSolutionPermuter&&) = default;

  iterator begin() const override { return iterator(absl::make_unique<Advancer>(this)); }
  iterator end() const override { return iterator(absl::make_unique<Advancer>(nullptr)); }

  double permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const {
    return class_permuters_[class_int];
  }

  bool AddPredicate(absl::string_view name, Solution::Predicate predicate,
		    const std::vector<int>& class_int_restrict_list) override;

  void Prepare() override;

 private:
  // Builds ActiveSet for each element in 'class_permuters_' (if flag enabled).
  // Elements in 'croppers' that are not completely evaluated by these active
  // sets are returned in 'residual'.
  void BuildActiveSets(std::vector<Solution::Cropper>* residual);

  // Reorders 'class_permuters_' by increasing selectivity. The effect of this
  // is to mean that any filter evaluated on a partial set of 'class_permuters_'
  // maximally prunes unnecessary iteration.
  void ReorderEvaluation();

  const EntryDescriptor* entry_descriptor_ = nullptr;

  std::vector<Solution::Cropper> predicates_;
  
  Profiler* profiler_;

  bool prepared_ = false;

  // Ordered by the evaluation order that is configured for 'class_predicates_'.
  // That is, if the first N permuters have been updated then permuting entries
  // N+1 and further should only be performed if
  // 'class_predicates_[class_permuter_[N-1].class_int()]' is true.
  std::vector<ClassPermuter> class_permuters_;

  // Index is class_int at which evaluation should be performed.
  std::vector<absl::optional<Solution::Cropper>> class_predicates_;

  std::unique_ptr<ActiveSetBuilder> active_set_builder_;

  friend Advancer;
};

}  // namespace puzzle

#endif  // __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
