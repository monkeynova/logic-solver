#ifndef __PUZZLE_CLASS_PERMUTER_H
#define __PUZZLE_CLASS_PERMUTER_H

#include "puzzle/solution.h"

namespace Puzzle {

class ClassPermuter {
 public:
  class iterator {
  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef ClassPermuter value_type;
    typedef ClassPermuter& reference;
    typedef ClassPermuter* pointer;

    iterator() : iterator(nullptr) {}
    iterator(const Descriptor* descriptor);

    iterator(const iterator&) = delete;
    iterator& operator=(const iterator&) = delete;

    iterator(iterator&&) = default;
    iterator& operator=(iterator&&) = default;

    bool operator!=(const iterator& other) {
      return !(*this == other);
    }
    bool operator==(const iterator& other) {
      return current_ == other.current_;
    }
    const std::vector<int>& operator*() {
      return current_;
    }
    const std::vector<int>* operator->() {
      return &current_;
    }
    iterator& operator++() {
      Advance();
      return *this;
    }
    
    double position() const { return position_; }
    double completion() const { return position_ / max_; }

   private:
    void Advance();

    std::vector<int> values_;
    std::vector<int> current_;
    std::vector<int> index_;
    std::vector<int> direction_;
    int next_from_;
    double position_;
    int max_;
  };

  ClassPermuter(const Descriptor* d)
    : descriptor_(d),
      permutation_count_(PermutationCount(d)) {}
  ~ClassPermuter() {}

  iterator begin() const { return iterator(descriptor_); }
  iterator end() const { return iterator(); }

  double permutation_count() const {
    return permutation_count_;
  }

 private:
  static double PermutationCount(const Descriptor* d);
  const Descriptor* descriptor_;
  double permutation_count_;
};

}  // namespace Puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
