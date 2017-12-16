#ifndef __PUZZLE_CLASS_PERMUTER_H
#define __PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set.h"
#include "puzzle/solution.h"

namespace puzzle {
  
class ClassPermuter {
 public:
  class iterator {
   public:
    constexpr static int kInlineSize = 10;
    using StorageVector = std::vector<int>;

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef ClassPermuter value_type;
    typedef ClassPermuter& reference;
    typedef ClassPermuter* pointer;

    iterator() : iterator(nullptr, {}) {}
    iterator(const Descriptor* descriptor, ActiveSet active_set);

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
    const StorageVector& operator*() {
      return current_;
    }
    const StorageVector* operator->() {
      return &current_;
    }
    iterator& operator++() {
      AdvanceWithSkip();
      return *this;
    }
    
    double position() const { return position_; }
    double completion() const { return position_ / max_; }

   private:
    // Advances permutation until the the result should be allowed considering
    // 'active_set_'.
    void AdvanceWithSkip();

    // Advances permutation exactly once independent of skipping behavior.
    void Advance();

    // Advances until the current record should allowed considering 'active_set_'.
    void SkipUntilMatch();

    StorageVector current_;
    StorageVector index_;
    StorageVector direction_;
    int next_from_;
    double position_;
    int max_;

    ActiveSet active_set_;
  };

  ClassPermuter(const Descriptor* d)
    : descriptor_(d),
      permutation_count_(PermutationCount(d)) {}
  ~ClassPermuter() {}

  iterator begin(ActiveSet active_set = {}) const {
    return iterator(descriptor_, std::move(active_set));
  }
  iterator end() const { return iterator(); }

  double permutation_count() const {
    return permutation_count_;
  }

 private:
  static double PermutationCount(const Descriptor* d);
  const Descriptor* descriptor_;
  double permutation_count_;
};

}  // namespace puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
