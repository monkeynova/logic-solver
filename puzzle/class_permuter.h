#ifndef PUZZLE_CLASS_PERMUTER_H
#define PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set.h"
#include "puzzle/solution.h"

namespace puzzle {

class ClassPermuter {
 public:
  class AdvancerBase {
   public:
    // Argument type for operator+= to advance until a sepecific position in the
    // permutation changes values.
    struct ValueSkip {
      int value_index;
    };
    static_assert(sizeof(ValueSkip) < 16,
                  "ValueSkip is assumed to be small enough for pass-by-value "
                  "semantics.");

    AdvancerBase(const ClassPermuter* permuter, ActiveSet active_set);

    virtual std::unique_ptr<AdvancerBase> Clone() const = 0;

    virtual ~AdvancerBase() {}

    void Prepare();

    virtual void Advance() = 0;
    virtual void AdvanceDelta(int dist) = 0;
    virtual void AdvanceSkip(ValueSkip value_skip) = 0;

    void AdvanceWithSkip();

    virtual const absl::Span<const int>& current() const = 0;
    int position() const { return position_; }
    const ActiveSet& active_set() const { return active_set_; }

    int class_int() const { return class_int_; }
    int permutation_size() const { return permutation_size_; }
    int permutation_count() const { return permutation_count_; }

   protected:
    // Position in the iteration. Integer from 1 to number of permutations.
    // Represents the position independent of skipped values from 'active_set'.
    int position_;

    // Representation of the subset of the permutations to return.
    ActiveSet active_set_;

   private:
    // The number of elements being permuted.
    int permutation_size_;

    // The number of permutations iterated (permutation_size_!).
    int permutation_count_;

    int class_int_;
  };

  template <int kStorageSize>
  class AdvancerStaticStorage : public AdvancerBase {
   public:
    AdvancerStaticStorage(const ClassPermuter* permuter, ActiveSet active_set)
        : AdvancerBase(permuter, active_set),
          current_span_(absl::MakeSpan(current_)) {
      DCHECK_EQ(kStorageSize, permuter->descriptor()->Values().size());
      memcpy(current_, permuter->descriptor()->Values().data(),
             sizeof(current_));
    }

    const absl::Span<const int>& current() const final { return current_span_; }

    void AdvanceSkip(ValueSkip value_skip) override {
      int value = current_[value_skip.value_index];
      if (active_set_.is_trivial()) {
        while (!current_span_.empty() &&
               current_[value_skip.value_index] == value) {
          Advance();
        }
      } else {
        while (!current_span_.empty() &&
               current_[value_skip.value_index] == value) {
          AdvanceWithSkip();
        }
      }
    }

   protected:
    // The cached current value of iteration.
    int current_[kStorageSize];

    absl::Span<const int> current_span_;
  };

  class iterator {
   public:
    constexpr static int kInlineSize = 10;
    using ValueSkip = typename AdvancerBase::ValueSkip;

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef absl::Span<const int> value_type;
    typedef absl::Span<const int> reference;
    typedef const absl::Span<const int> const_reference;
    typedef absl::Span<const int>* pointer;
    typedef const absl::Span<const int>* const_pointer;

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
      advancer_->AdvanceSkip(value_skip);
      return *this;
    }

    int position() const { return advancer_->position(); }
    double Completion() const {
      return static_cast<double>(position()) / advancer_->permutation_count();
    }
    int class_int() const { return advancer_->class_int(); }

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

  explicit ClassPermuter(const Descriptor* d, int class_int)
      : descriptor_(d),
        permutation_count_(PermutationCount(d)),
        class_int_(class_int) {
    active_set_.DoneAdding();
  }
  virtual ~ClassPermuter() {}

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

inline std::ostream& operator<<(std::ostream& out,
                                const ClassPermuter& permuter) {
  return out << permuter.DebugString();
}

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_H
