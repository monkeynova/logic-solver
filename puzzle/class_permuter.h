#ifndef __PUZZLE_CLASS_PERMUTER_H
#define __PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set.h"
#include "puzzle/solution.h"

namespace puzzle {
namespace internal {

enum class ClassPermuterType {
  kUnknown = 0,
    // https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
  kSteinhausJohnsonTrotter = 1,
  kFactorialRadix = 2,
};

template <enum ClassPermuterType T>
class ClassPermuterImpl {
 public:
  class iterator {
   public:
    constexpr static int kInlineSize = 10;
    using StorageVector = std::vector<int>;

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef StorageVector value_type;
    typedef StorageVector& reference;
    typedef const StorageVector& const_reference;
    typedef StorageVector* pointer;
    typedef const StorageVector* const_pointer;

    iterator() : iterator(nullptr, {}) {}
    iterator(const ClassPermuterImpl<T>* permuter,
             ActiveSet active_set);

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
    const_reference operator*() const {
      return current_;
    }
    const_pointer operator->() const {
      return &current_;
    }
    iterator& operator++() {
      if (active_set_.is_trivial()) {
        Advance();
      } else {
        AdvanceWithSkip();
      }
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

    // Advances permutation 'dist' positions  independent of skipping behavior.
    void Advance(int dist);

    // Equivalent to Advance(1).
    void Advance();

    // Initializes Algorithm depend information during construction.
    void InitIndex();

    // Permuter iteration is being performed on.
    const ClassPermuterImpl<T>* permuter_;

    // The cached current value of iteration.
    value_type current_;

    // Algorithm dependent information for iteration.
    value_type index_;
    value_type direction_;
    int next_from_;

    // Position in the iteration. Integer from 1 to number of permutations.
    // Represents the position independent of skipped values from 'active_set'.
    double position_;

    // Representation of the subset of the permutations to return.
    ActiveSet active_set_;
  };

 explicit ClassPermuterImpl(const Descriptor* d = nullptr,
                            const int class_int = 0)
    : descriptor_(d),
      permutation_count_(PermutationCount(d)),
      class_int_(class_int) {
    active_set_.DoneAdding();
  }
  ~ClassPermuterImpl() {}

  // Moveable but not copyable.
  ClassPermuterImpl(const ClassPermuterImpl&) = delete;
  ClassPermuterImpl& operator=(const ClassPermuterImpl&) = delete;
  ClassPermuterImpl(ClassPermuterImpl&&) = default;
  ClassPermuterImpl& operator=(ClassPermuterImpl&&) = default;

  // TODO(keith): This copy of active_set_ is likely the cause of malloc
  // showing up on profiles. We should clean up the model to avoid needing
  // a data copy here.
  iterator begin() const {
    return iterator(this, active_set_);
  }
  iterator begin(ActiveSet active_set) const {
    return iterator(this, std::move(active_set_));
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

  const ActiveSet& active_set() const {
    return active_set_;
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

}  // namespace internal

using ClassPermuter = internal::ClassPermuterImpl<
    internal::ClassPermuterType::kFactorialRadix>;

}  // namespace puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
