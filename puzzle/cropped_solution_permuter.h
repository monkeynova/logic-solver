#ifndef __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
#define __PUZZLE_CROPPED_SOLUTION_PERMUTER_H

#include "puzzle/active_set_builder.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/profiler.h"
#include "puzzle/solution.h"

namespace puzzle {

class CroppedSolutionPermuter {
 public:
  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef Solution value_type;
    typedef Solution& reference;
    typedef Solution* pointer;

    iterator(const CroppedSolutionPermuter* permuter);

    iterator(const iterator& other) = delete;
    iterator& operator=(const iterator& other) = delete;

    iterator(iterator&& other) = default;
    iterator& operator=(iterator&& other) = default;

    bool operator!=(const iterator& other) {
      return !(*this == other);
    }
    bool operator==(const iterator& other) {
      return current_ == other.current_;
    }
    const Solution& operator*() { return current_; }
    const Solution* operator->() { return &current_; }
    iterator& operator++() {
      Advance();
      return *this;
    }

    double position() const;
    double completion() const;

  private:
    void PruneClass(int class_int,
                    const std::vector<Solution::Cropper>& predicates);
    void Advance();
    bool FindNextValid(int class_position);

    // If permuter_->profiler is not null, calls NotePosition and flushes a
    // status prompt to std::out. Returns true if the profiler signals an
    // early abort is indicated.
    bool NotePositionForProfiler(int class_position);

    const CroppedSolutionPermuter* const permuter_ = nullptr;
    MutableSolution mutable_solution_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;  // Bound to mutable_solution_.
  };

  CroppedSolutionPermuter(
      const EntryDescriptor* e,
      const std::vector<Solution::Cropper>& croppers_with_class,
      Profiler* profiler);
  ~CroppedSolutionPermuter() {}

  // Movable, but not copyable.
  CroppedSolutionPermuter(const CroppedSolutionPermuter&) = delete;
  CroppedSolutionPermuter& operator=(const CroppedSolutionPermuter&) = delete;
  CroppedSolutionPermuter(CroppedSolutionPermuter&&) = default;
  CroppedSolutionPermuter& operator=(CroppedSolutionPermuter&&) = default;

  iterator begin() const { return iterator(this); }
  iterator end() const { return iterator(nullptr); }

  double permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const {
    return class_permuters_[class_int];
  }

 private:
  // Builds ActiveSet for each element in 'class_permuters_' (if flag enabled).
  // Elements in 'croppers' that are not completely evaluated by these active
  // sets are returned in 'residual'.
  void BuildActiveSets(const std::vector<Solution::Cropper>& croppers,
                       std::vector<Solution::Cropper>* residual);

  // Reorders 'class_permuters_' by increasing selectivity. The effect of this
  // is to mean that any filter evaluated on a partial set of 'class_permuters_'
  // maximally prunes unnecessary iteration.
  void ReorderEvaluation();

  const EntryDescriptor* const entry_descriptor_ = nullptr;

  // Ordered by the evaluation order that is configured for 'class_predicates_'.
  // That is, if the first N permuters have been updated then permuting entries
  // N+1 and further should only be performed if
  // 'class_predicates_[class_permuter_[N-1].class_int()]' is true.
  std::vector<ClassPermuter> class_permuters_;

  // Index is class_int at which evaluation should be performed.
  std::vector<Solution::Cropper> class_predicates_;

  Profiler* const profiler_;

  ActiveSetBuilder active_set_builder_;

  friend iterator;
};

}  // namespace puzzle

#endif  // __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
