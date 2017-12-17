#ifndef __PUZZLE_BRUTE_SOLUTION_PERMUTER_H
#define __PUZZLE_BRUTE_SOLUTION_PERMUTER_H

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"

namespace puzzle {

class BruteSolutionPermuter {
 public:
  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef Solution value_type;
    typedef Solution& reference;
    typedef Solution* pointer;

    iterator(const BruteSolutionPermuter* permuter,
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

    double position() const;
    double completion() const;

   private:
    void Advance();

    const BruteSolutionPermuter* permuter_;
    const EntryDescriptor* entry_descriptor_;
    std::vector<Entry> entries_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;
  };

  BruteSolutionPermuter(const EntryDescriptor* e);
  ~BruteSolutionPermuter() {}

  iterator begin() const { return iterator(this, entry_descriptor_); }
  iterator end() const { return iterator(this, nullptr); }

  double permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const {
    return class_permuters_[class_int];
  }

 private:
  const EntryDescriptor* entry_descriptor_;
  std::vector<ClassPermuter> class_permuters_;

  friend iterator;
};

}  // namespace puzzle

#endif  // __PUZZLE_BRUTE_SOLUTION_PERMUTER_H
