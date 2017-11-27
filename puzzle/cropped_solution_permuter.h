#ifndef __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
#define __PUZZLE_CROPPED_SOLUTION_PERMUTER_H

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"

namespace Puzzle {

class CroppedSolutionPermuter {
 public:
  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef CroppedSolutionPermuter value_type;
    typedef CroppedSolutionPermuter& reference;
    typedef CroppedSolutionPermuter* pointer;

    iterator(const CroppedSolutionPermuter* permuter,
             const EntryDescriptor* entry_descriptor);

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

    long long position() const;
    double completion() const;

  private:
    void Advance();
    bool FindNextValid(int class_position);
    void UpdateEntries(int class_int);

    const CroppedSolutionPermuter* permuter_;
    const EntryDescriptor* entry_descriptor_;
    std::vector<Entry> entries_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;
  };

  CroppedSolutionPermuter(const EntryDescriptor* e,
                          const std::vector<SolutionCropper>& croppers_with_class);
  ~CroppedSolutionPermuter() {}

  iterator begin() const { return iterator(this, entry_descriptor_); }
  iterator end() const { return iterator(this, nullptr); }

  long long permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const { return class_permuters_[class_int]; }

 private:
  const EntryDescriptor* entry_descriptor_;
  std::vector<ClassPermuter> class_permuters_;
  std::vector<std::vector<SolutionCropper>> class_crop_predicates_;

  friend iterator;
};

}  // namespace Puzzle

#endif  // __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
