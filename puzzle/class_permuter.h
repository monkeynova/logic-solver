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
    iterator(const ClassPermuter* permuter, ActiveSet active_set);

    iterator(const iterator&) = delete;
    iterator& operator=(const iterator&) = delete;

    iterator(iterator&&) = default;
    iterator& operator=(iterator&&) = default;

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }
    bool operator==(const iterator& other) const {
      return current_ == other.current_;
    }
    const StorageVector& operator*() const {
      return current_;
    }
    const StorageVector* operator->() const {
      return &current_;
    }
    iterator& operator++() {
      AdvanceWithSkip();
      return *this;
    }
    
    double position() const { return position_; }
    double Completion() const {
      return position_ / permuter_->permutation_count();
    }
    int class_int() const { return permuter_->class_int(); }
    
   private:
    // Advances permutation until the the result should be allowed considering
    // 'active_set_'.
    void AdvanceWithSkip();

    // Advances permutation exactly once independent of skipping behavior.
    void Advance();

    // Advances until the current record should allowed considering
    // 'active_set_'.
    void SkipUntilMatch();

    const ClassPermuter* permuter_;
    StorageVector current_;
    StorageVector index_;
    StorageVector direction_;
    int next_from_;
    double position_;

    ActiveSet active_set_;
  };

 explicit ClassPermuter(const Descriptor* d = nullptr, const int class_int = 0)
    : descriptor_(d),
      permutation_count_(PermutationCount(d)),
      class_int_(class_int) {}
  ~ClassPermuter() {}

  // Moveable but not copyable.
  ClassPermuter(const ClassPermuter&) = delete;
  ClassPermuter& operator=(const ClassPermuter&) = delete;
  ClassPermuter(ClassPermuter&&) = default;
  ClassPermuter& operator=(ClassPermuter&&) = default;

  iterator begin() const {
    return iterator(this, active_set_);
  }
  iterator end() const { return iterator(); }

  double permutation_count() const {
    return permutation_count_;
  }

  const Descriptor* descriptor() const { return descriptor_; }
  
  int class_int() const {
    return class_int_;
  }

  // TODO(keith@monkeynova.com): Materialize full permutation if ActiveSet is
  // selective enough.
  void set_active_set(ActiveSet active_set) {
    active_set_ = std::move(active_set);
  }

  double Selectivity() const {
    return active_set_.Selectivity();
  }
  
 private:
  static double PermutationCount(const Descriptor* d);
  const Descriptor* descriptor_;
  double permutation_count_;
  int class_int_;
  ActiveSet active_set_;
};

}  // namespace puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
