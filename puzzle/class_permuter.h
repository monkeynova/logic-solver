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

    explicit AdvancerBase(const ClassPermuter* permuter);

    // Non-default copy constructor to maintain `active_set_it_` as a reference
    // to `active_set_`.
    AdvancerBase(const AdvancerBase& other);

    virtual std::unique_ptr<AdvancerBase> Clone() const = 0;

    virtual ~AdvancerBase() {}

    virtual void Advance() = 0;
    virtual void AdvanceDelta(int dist) = 0;
    virtual void AdvanceSkip(ValueSkip value_skip) = 0;

    void AdvanceWithSkip();

    virtual const absl::Span<const int>& current() const = 0;
    int position() const { return position_; }
    const ActiveSet& active_set() const { return active_set_; }

    int class_int() const { return class_int_; }
    virtual int permutation_size() const = 0;
    int permutation_count() const { return permutation_count_; }

    // Intersects the current active set with the new addition. If this
    // intersection invalidates the current record, advances the iterator to
    // the next matching record. Returns whether or not the iterator was
    // advanced.
    bool WithActiveSet(const ActiveSet& active_set);

    double Selectivity() const { return active_set_.Selectivity(); }

   protected:
    // Position in the iteration. Integer from 1 to number of permutations.
    // Represents the position independent of skipped values from 'active_set'.
    int position_;

    // Representation of the subset of the permutations to return.
    ActiveSet active_set_;
    ActiveSetIterator active_set_it_;

   private:
    // The number of permutations iterated (permutation_size_!).
    int permutation_count_;

    int class_int_;
  };

  template <int kStorageSize>
  class AdvancerStaticStorage : public AdvancerBase {
   public:
    AdvancerStaticStorage(const ClassPermuter* permuter)
        : AdvancerBase(permuter), current_span_(absl::MakeSpan(current_)) {
      DCHECK_EQ(kStorageSize, permuter->values().size());
      memcpy(current_, permuter->values().data(), sizeof(current_));
    }

    // Explicity copy constructor so current_span_ points to this->current
    // rather than other.current, which would be the default implementation.
    AdvancerStaticStorage(const AdvancerStaticStorage<kStorageSize>& other)
        : AdvancerBase(other),
          current_span_(  // Preserve is_end().
              other.current_span_.empty() ? absl::Span<const int>()
                                          : absl::MakeSpan(current_)) {
      memcpy(current_, other.current_, sizeof(current_));
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

    int permutation_size() const final { return kStorageSize; }

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
        : advancer_(std::move(advancer)) {}

    iterator(const iterator& other) : iterator(other.advancer_->Clone()) {}
    iterator& operator=(const iterator& other) {
      advancer_ = other.advancer_->Clone();
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
    // TODO(@monkeynova): The details of permutation iteration are
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

    iterator& WithActiveSet(const ActiveSet& active_set,
                            bool* was_advanced_out = nullptr) {
      if (!active_set.is_trivial()) {
        const bool was_advanced = advancer_->WithActiveSet(active_set);
        if (was_advanced_out != nullptr) {
          *was_advanced_out = was_advanced;
        }
      }
      return *this;
    }

    double Selectivity() const { return advancer_->Selectivity(); }

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
        values_(d->Values()),
        permutation_count_(PermutationCount(d)),
        class_int_(class_int) {}
  virtual ~ClassPermuter() {}

  virtual iterator begin() const = 0;

  iterator end() const { return iterator(); }

  int permutation_size() const { return values_.size(); }
  int permutation_count() const { return permutation_count_; }

  const Descriptor* descriptor() const { return descriptor_; }
  const std::vector<int>& values() const { return values_; }

  int class_int() const { return class_int_; }

  std::string DebugString() const;

 private:
  static int PermutationCount(const Descriptor* d);
  const Descriptor* descriptor_;
  std::vector<int> values_;
  int permutation_count_;
  int class_int_;
};

inline std::ostream& operator<<(std::ostream& out,
                                const ClassPermuter& permuter) {
  return out << permuter.DebugString();
}

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_H
