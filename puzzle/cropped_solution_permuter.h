#ifndef __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
#define __PUZZLE_CROPPED_SOLUTION_PERMUTER_H

#include "puzzle/class_permuter.h"
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
    const Solution& operator*() {
      return current_;
    }
    const Solution* operator->() {
      return &current_;
    }
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
    void UpdateEntries(int class_int);

    const CroppedSolutionPermuter* const permuter_ = nullptr;
    std::vector<Entry> entries_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;
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

  const std::vector<int>& class_order() const {
    return class_order_;
  }

 private:
  Solution BuildSolution(std::vector<Entry>* enries) const;
  
  const EntryDescriptor* const entry_descriptor_ = nullptr;
  std::vector<ClassPermuter> class_permuters_;

  std::vector<int> class_order_;
  
  // Index is larges (i.e. last) class needed to evaluate.
  // TODO(keith@monkeynova.com): Move this to a single Cropper capturing the
  // relevant predicates.
  std::vector<std::vector<Solution::Cropper>> multi_class_predicates_;

  Profiler* const profiler_;
  
  friend iterator;
};

}  // namespace puzzle

#endif  // __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
