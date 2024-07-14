#ifndef PUZZLE_CLASS_PERMUTER_H
#define PUZZLE_CLASS_PERMUTER_H

#include "puzzle/active_set/active_set.h"
#include "puzzle/base/all_match.h"

namespace puzzle {

class ClassPermuter {
 public:
  class AdvancerBase {
   public:
    explicit AdvancerBase(const ClassPermuter* permuter);

    // Non-default copy constructor to maintain `active_set_it_` as a reference
    // to `active_set_`.
    AdvancerBase(const AdvancerBase& other);

    virtual ~AdvancerBase() {
      if (active_set_owned_) delete active_set_;
    }

    virtual void Advance() = 0;
    virtual void AdvanceDelta(int dist) = 0;
    virtual void AdvanceSkip(ValueSkip value_skip) = 0;

    // Advances permutation until the the result should be allowed considering
    // 'active_set_'.
    void AdvanceWithSkip() { AdvanceDeltaWithSkip(/*delta=*/1); }
    void AdvanceDeltaWithSkip(int delta);

    const absl::Span<const int>& current() const { return current_; }
    bool done() const { return current_.empty(); }
    int position() const { return position_; }
    const ActiveSet& active_set() const { return *active_set_; }

    int class_int() const { return class_int_; }
    virtual int permutation_size() const = 0;
    int permutation_count() const { return permutation_count_; }

    // Intersects the current active set with the new addition. If this
    // intersection invalidates the current record, advances the iterator to
    // the next matching record. Returns whether or not the iterator was
    // advanced.
    bool WithActiveSet(const ActiveSet& active_set);

    double Selectivity() const { return active_set_->Selectivity(); }

   protected:
    void set_current(absl::Span<const int> current) { current_ = current; }

    // Position in the iteration. Integer from 1 to number of permutations.
    // Represents the position independent of skipped values from 'active_set'.
    int position_;

    // Representation of the subset of the permutations to return.
    const ActiveSet* active_set_;
    ActiveSet::Iterator active_set_it_;

   private:
    absl::Span<const int> current_;

    bool active_set_owned_;

    // The number of permutations iterated (permutation_size_!).
    int permutation_count_;

    int class_int_;
  };

  template <int kStorageSize>
  class AdvancerStaticStorage : public AdvancerBase {
   public:
    AdvancerStaticStorage(const ClassPermuter* permuter)
        : AdvancerBase(permuter) {
      set_current(absl::MakeSpan(current_));
      DCHECK_EQ(kStorageSize, permuter->permutation_size());
      std::iota(current_, current_ + kStorageSize, 0);
    }

    // Explicity copy constructor so current_ points to this->current
    // rather than other.current, which would be the default implementation.
    AdvancerStaticStorage(const AdvancerStaticStorage<kStorageSize>& other)
        : AdvancerBase(other) {
      set_current(other.done() ? absl::Span<const int>()
                               : absl::MakeSpan(current_));
      memcpy(current_, other.current_, sizeof(current_));
    }

    void AdvanceSkip(ValueSkip value_skip) override {
      int value = current_[value_skip.value_index];
      if (active_set_->is_trivial()) {
        while (!done() && current_[value_skip.value_index] == value) {
          Advance();
        }
      } else {
        while (!done() && current_[value_skip.value_index] == value) {
          AdvanceWithSkip();
        }
      }
    }

    int permutation_size() const final { return kStorageSize; }

   protected:
    // The cached current value of iteration.
    int current_[kStorageSize];
  };

  class iterator {
   public:
    constexpr static int kInlineSize = 10;

    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef absl::Span<const int> value_type;
    typedef absl::Span<const int> reference;
    typedef const absl::Span<const int> const_reference;
    typedef absl::Span<const int>* pointer;
    typedef const absl::Span<const int>* const_pointer;

    explicit iterator(std::unique_ptr<AdvancerBase> advancer = nullptr)
        : advancer_(std::move(advancer)) {}

    // Movable, but not copyable. Implicit copies were hard to track down while
    // enabled, and noticably costly on profiles.
    // Unfortunately this disallows absl::StrJoin(class_permuter, ","), but "que
    // sera, sera".
    iterator(const iterator&) = delete;
    iterator& operator=(const iterator&) = delete;

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

    iterator& operator+=(int delta) {
      if (advancer_->active_set().is_trivial()) {
        advancer_->AdvanceDelta(delta);
      } else {
        advancer_->AdvanceDeltaWithSkip(delta);
      }
      return *this;
    }

    // Advance until the value of `current_[value_skip.value_index]` changes.
    iterator& operator+=(ValueSkip value_skip) {
      if (value_skip.value_index == Entry::kBadId) {
        return ++*this;
      }
      advancer_->AdvanceSkip(value_skip);
      return *this;
    }

    int position() const {
      return advancer_ == nullptr ? 0 : advancer_->position();
    }
    double Completion() const {
      if (is_end()) return 1.0;
      return static_cast<double>(advancer_->position()) /
             advancer_->permutation_count();
    }
    int class_int() const { return advancer_->class_int(); }

    // Restricts this iterator to only return positions marked as true within
    // `active_set`. Returns in `was_advanced` whether or not `active_set`
    // invalidated the current location and the iterator was advanced to match.
    // WARNING: A reference to `active_set` is saved and it must outlive this
    // iterator.
    iterator&& WithActiveSet(const ActiveSet& active_set,
                             bool* was_advanced = nullptr) && {
      if (!active_set.is_trivial()) {
        const bool was_advanced_tmp = advancer_->WithActiveSet(active_set);
        if (was_advanced != nullptr) {
          *was_advanced = was_advanced_tmp;
        }
      } else if (was_advanced != nullptr) {
        *was_advanced = false;
      }
      return std::move(*this);
    }

    double Selectivity() const { return advancer_->Selectivity(); }

   private:
    bool is_end() const { return advancer_ == nullptr || advancer_->done(); }

    // Implementation dependent means of advancing through permutations.
    std::unique_ptr<AdvancerBase> advancer_;
  };

  explicit ClassPermuter(int permutation_size, int class_int)
      : permutation_size_(permutation_size),
        permutation_count_(PermutationCount(permutation_size_)),
        class_int_(class_int) {}
  virtual ~ClassPermuter() = default;

  virtual iterator begin() const = 0;

  iterator end() const { return iterator(); }

  int permutation_size() const { return permutation_size_; }
  int permutation_count() const { return permutation_count_; }

  const Descriptor* descriptor() const { return descriptor_; }

  int class_int() const { return class_int_; }

  std::string DebugString() const;

 private:
  static int PermutationCount(int permutation_size);
  const Descriptor* descriptor_;
  int permutation_size_;
  int permutation_count_;
  int class_int_;
};

inline std::ostream& operator<<(std::ostream& out,
                                const ClassPermuter& permuter) {
  return out << permuter.DebugString();
}

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_H
