#ifndef PUZZLE_CLASS_PERMUTER_H
#define PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set.h"
#include "puzzle/solution.h"

namespace puzzle {
namespace internal {

enum class ClassPermuterType {
  kUnknown = 0,
  // https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
  kSteinhausJohnsonTrotter = 1,
  // Treats the permutation index as a factorial radix number
  // ({0..8} * 8! + {0..7} * 7! + ... {0..1} * 1! + {0} * 0!).
  // This implementation is O(class_size) turning a position into a permutation
  // but does not allow seeking to a position for Advance(ValueSkip).
  kFactorialRadix = 2,
  // This implementation is O(class_size^2) turning a position into a
  // permutation but does allows a single position advance for
  // Advance(ValueSkip).
  kFactorialRadixDeleteTracking = 3,
};

template <enum ClassPermuterType T>
class ClassPermuterImpl {
 public:
  class iterator {
   public:
    constexpr static int kInlineSize = 10;
    using StorageVector = std::vector<int>;

    // Argument type for operator+= to advance until a sepecific position in the
    // permutation changes values.
    struct ValueSkip {
      int value_index;
    };

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef StorageVector value_type;
    typedef StorageVector& reference;
    typedef const StorageVector& const_reference;
    typedef StorageVector* pointer;
    typedef const StorageVector* const_pointer;

    iterator() : iterator(nullptr, {}) {}
    iterator(const ClassPermuterImpl<T>* permuter, ActiveSet active_set);

    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = default;

    bool operator!=(const iterator& other) const { return !(*this == other); }
    bool operator==(const iterator& other) const {
      return current_ == other.current_;
    }
    const_reference operator*() const { return current_; }
    const_pointer operator->() const { return &current_; }
    iterator& operator++() {
      if (active_set_.is_trivial()) {
        Advance();
      } else {
        AdvanceWithSkip();
      }
      return *this;
    }

    // Advance until the value of `current_[value_skip.value_index]` changes.
    // TODO(keith@monkeynova.com): The details of permutation iteration are
    // putting the iteration reduction to a permutation of size 9 between 1-4x
    // when skipping at a single value_index through the full permutation. If
    // our goal is to skip all permutations with a specific (index, value) pair,
    // we should be able to skip up to 8! permutations.
    iterator& operator+=(ValueSkip value_skip) {
      if (value_skip.value_index == Entry::kBadId) {
        return ++*this;
      }
      Advance(value_skip);
      return *this;
    }

    double position() const { return static_cast<double>(position_); }
    double Completion() const {
      return static_cast<double>(position_) / permuter_->permutation_count();
    }
    int class_int() const { return permuter_->class_int(); }

   private:
    // Advances permutation until the the result should be allowed considering
    // 'active_set_'.
    void AdvanceWithSkip();

    // Advances permutation 'dist' positions  independent of skipping behavior.
    void Advance(int dist);

    void Advance(ValueSkip value_skip);

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
    int position_;

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

  ClassPermuterImpl(const ClassPermuterImpl&) = default;
  ClassPermuterImpl& operator=(const ClassPermuterImpl&) = default;

  // TODO(keith): This copy of active_set_ is likely the cause of malloc
  // showing up on profiles. We should clean up the model to avoid needing
  // a data copy here.
  iterator begin() const { return iterator(this, active_set_); }
  iterator begin(ActiveSet active_set) const {
    return iterator(this, std::move(active_set));
  }
  iterator end() const { return iterator(); }

  double permutation_count() const { return permutation_count_; }

  const Descriptor* descriptor() const { return descriptor_; }

  int class_int() const { return class_int_; }

  // TODO(keith@monkeynova.com): Materialize full permutation if ActiveSet is
  // selective enough.
  void set_active_set(ActiveSet active_set) {
    active_set_ = std::move(active_set);
  }

  const ActiveSet& active_set() const { return active_set_; }

  double Selectivity() const { return active_set_.Selectivity(); }

  std::string DebugString() const;

 private:
  static double PermutationCount(const Descriptor* d);
  const Descriptor* descriptor_;
  double permutation_count_;
  int class_int_;
  ActiveSet active_set_;
};

template <enum ClassPermuterType T>
std::ostream& operator<<(std::ostream& out,
                         internal::ClassPermuterImpl<T> permuter) {
  return out << permuter.DebugString();
}

}  // namespace internal

using ClassPermuter =
    internal::ClassPermuterImpl<internal::ClassPermuterType::kFactorialRadix>;

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_H
