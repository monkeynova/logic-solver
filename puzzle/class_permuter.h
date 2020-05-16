#ifndef PUZZLE_CLASS_PERMUTER_H
#define PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set.h"
#include "puzzle/solution.h"

namespace puzzle {
namespace internal {

// Contains a map from a radix index position and a bit vector marked with
// previously selected values from index_ in a permutation to the index for
// the correspondingly selected value in index_.
// This is a lookup-table for the function ComputeRadixIndexToRawIndex which
// is called in the innermost loop.
using RadixIndexToRawIndex = std::vector<std::vector<int>>;

class ClassPermuterBase {
 public:
  class AdvancerBase {
  public:
    using StorageVector = std::vector<int>;

    // Argument type for operator+= to advance until a sepecific position in the
    // permutation changes values.
    struct ValueSkip {
      int value_index;
    };

    AdvancerBase(const ClassPermuterBase* permuter, ActiveSet active_set);

    virtual std::unique_ptr<AdvancerBase> Clone() const = 0;
    
    virtual ~AdvancerBase() {}

    void Prepare();

    virtual void Advance() = 0;
    virtual void Advance(int dist) = 0;
    virtual void Advance(ValueSkip value_skip) = 0;

    void AdvanceWithSkip();

    const ClassPermuterBase* permuter() const { return permuter_; }
    const StorageVector& current() const { return current_; }
    int position() const { return position_; }
    const ActiveSet& active_set() const { return active_set_; }
    
   protected:
    const ClassPermuterBase* permuter_;

    // The cached current value of iteration.
    StorageVector current_;

    // Position in the iteration. Integer from 1 to number of permutations.
    // Represents the position independent of skipped values from 'active_set'.
    int position_;

    // Representation of the subset of the permutations to return.
    ActiveSet active_set_;
  };
  
  class iterator {
   public:
    constexpr static int kInlineSize = 10;
    using StorageVector = typename AdvancerBase::StorageVector;
    using ValueSkip = typename AdvancerBase::ValueSkip;

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef StorageVector value_type;
    typedef StorageVector& reference;
    typedef const StorageVector& const_reference;
    typedef StorageVector* pointer;
    typedef const StorageVector* const_pointer;

    explicit iterator(std::unique_ptr<AdvancerBase> advancer = nullptr)
      : advancer_(std::move(advancer)) {
      if (advancer_ != nullptr) advancer_->Prepare();
    }

    iterator(const iterator& other) : iterator(other.advancer_->Clone()) {}
    iterator& operator=(const iterator& other) {
      advancer_ = other.advancer_->Clone();
      if (advancer_ != nullptr) advancer_->Prepare();
      return *this;
    }
    
    iterator(iterator&&) = default;
    iterator& operator=(iterator&&) = default;

    bool operator!=(const iterator& other) const { return !(*this == other); }
    bool operator==(const iterator& other) const {
      if (is_end()) return other.is_end();
      if (other.is_end()) return false;
      return advancer_->current() == other.advancer_->current();
    }
    const_reference operator*() const { return advancer_->current(); }
    const_pointer operator->() const { return &advancer_->current(); }
    iterator& operator++() {
      if (advancer_->active_set().is_trivial()) {
        advancer_->Advance();
      } else {
        advancer_->AdvanceWithSkip();
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
      advancer_->Advance(value_skip);
      return *this;
    }

    int position() const { return advancer_->position(); }
    double Completion() const {
      return static_cast<double>(position()) / advancer_->permuter()->permutation_count();
    }
    int class_int() const { return advancer_->permuter()->class_int(); }

   private:
    bool is_end() const {
      return advancer_ == nullptr || advancer_->current().empty();
    }
    
    // Advances permutation until the the result should be allowed considering
    // 'active_set_'.
    void AdvanceWithSkip();

    // Implementation dependent means of advancing through permutations.
    std::unique_ptr<AdvancerBase> advancer_;
  };

  explicit ClassPermuterBase(const Descriptor* d = nullptr,
                             const int class_int = 0)
      : descriptor_(d),
        permutation_count_(PermutationCount(d)),
        class_int_(class_int) {
    active_set_.DoneAdding();
  }
  virtual ~ClassPermuterBase() {}

  // TODO(keith): This copy of active_set_ is likely the cause of malloc
  // showing up on profiles. We should clean up the model to avoid needing
  // a data copy here.
  virtual iterator begin() const = 0;
  virtual iterator begin(ActiveSet active_set) const = 0;

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

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
class ClassPermuterSteinhausJohnsonTrotter final : public ClassPermuterBase {
 public:
  class Advancer final : public AdvancerBase {
  public:
    Advancer(const ClassPermuterSteinhausJohnsonTrotter* permuter, ActiveSet active_set);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void Advance(int dist) override;
    void Advance(ValueSkip value_skip) override;

  private:
    // Algorithm dependent information for iteration.
    StorageVector index_;
    StorageVector direction_;
    int next_from_;
  };

  explicit ClassPermuterSteinhausJohnsonTrotter(const Descriptor* d = nullptr,
						const int class_int = 0)
    : ClassPermuterBase(d, class_int) {}

  ClassPermuterSteinhausJohnsonTrotter(ClassPermuterSteinhausJohnsonTrotter&&) = default;
  ClassPermuterSteinhausJohnsonTrotter& operator=(ClassPermuterSteinhausJohnsonTrotter&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

// Treats the permutation index as a factorial radix number
// ({0..8} * 8! + {0..7} * 7! + ... {0..1} * 1! + {0} * 0!).
// This implementation is O(class_size) turning a position into a permutation
// but does not allow seeking to a position for Advance(ValueSkip).
class ClassPermuterFactorialRadix final : public ClassPermuterBase {
 public:
  class Advancer final : public AdvancerBase {
  public:
    Advancer(const ClassPermuterFactorialRadix* permuter, ActiveSet active_set);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void Advance(int dist) override;
    void Advance(ValueSkip value_skip) override;

   private:
    StorageVector index_;
  };

  explicit ClassPermuterFactorialRadix(const Descriptor* d = nullptr,
						const int class_int = 0)
    : ClassPermuterBase(d, class_int) {}

  ClassPermuterFactorialRadix(ClassPermuterFactorialRadix&&) = default;
  ClassPermuterFactorialRadix& operator=(ClassPermuterFactorialRadix&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

// This implementation is O(class_size^2) turning a position into a
// permutation but does allows a single position advance for
// Advance(ValueSkip).
class ClassPermuterFactorialRadixDeleteTracking final : public ClassPermuterBase {
 public:
  class Advancer final : public AdvancerBase {
  public:
    Advancer(const ClassPermuterFactorialRadixDeleteTracking* permuter, ActiveSet active_set);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void Advance(int dist) override;
    void Advance(ValueSkip value_skip) override;

   private:
    StorageVector index_;

    // Memory based data structure to turn an O(N^2) delete with replacement
    // into an O(N) one.
    RadixIndexToRawIndex* radix_index_to_raw_index_;
  };

  explicit ClassPermuterFactorialRadixDeleteTracking(const Descriptor* d = nullptr,
						const int class_int = 0)
    : ClassPermuterBase(d, class_int) {}

  ClassPermuterFactorialRadixDeleteTracking(ClassPermuterFactorialRadixDeleteTracking&&) = default;
  ClassPermuterFactorialRadixDeleteTracking& operator=(ClassPermuterFactorialRadixDeleteTracking&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

inline std::ostream& operator<<(std::ostream& out, const ClassPermuterBase& permuter) {
  return out << permuter.DebugString();
}

}  // namespace internal

using ClassPermuter = internal::ClassPermuterFactorialRadixDeleteTracking;

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_H
