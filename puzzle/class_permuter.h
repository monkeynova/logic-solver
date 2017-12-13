#ifndef __PUZZLE_CLASS_PERMUTER_H
#define __PUZZLE_CLASS_PERMUTER_H

#include "puzzle/solution.h"

namespace Puzzle {

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
    iterator(const Descriptor* descriptor, std::vector<int> skips);

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
    // 'skips_'.
    void AdvanceWithSkip();

    // Advances permutation exactly once independent of skipping behavior.
    void Advance();

    // Advances until the current record should allowed considering 'skips_'.
    void SkipUntilMatch();

    // Returns whether or not to skip the current record and advances index
    // structures through skips_.
    bool ConsumeNextSkip();

    StorageVector current_;
    StorageVector index_;
    StorageVector direction_;
    int next_from_;
    double position_;
    int max_;

    // Thar be dragons here.
    // 'skips_' is a vector of ints represneting runs of boolean conditions.
    // The first element corresponds to a run of "true" (i.e. should return)
    // permutations and each subsequent element negates the logic of the
    // previous run.
    // To start a run with "false", insert a 0 record at the first position.
    std::vector<int> skips_;
    bool skip_match_ = true;
    int skips_position_ = 0;
  };

  ClassPermuter(const Descriptor* d)
    : descriptor_(d),
      permutation_count_(PermutationCount(d)) {}
  ~ClassPermuter() {}

  iterator begin(std::vector<int> skips = {}) const {
    return iterator(descriptor_, std::move(skips));
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

}  // namespace Puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
