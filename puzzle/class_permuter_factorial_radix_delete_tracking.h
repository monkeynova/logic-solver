#ifndef PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H
#define PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// Contains a map from a radix index position and a bit vector marked with
// previously selected values from index_ in a permutation to the index for
// the correspondingly selected value in index_.
// This is a lookup-table for the function RadixIndexToRawIndex::ComputeValue
// which is needed in the innermost loop of permutation calculation.
template <int kMaxPos>
class RadixIndexToRawIndex {
 public:
  static RadixIndexToRawIndex<kMaxPos>* Singleton();

  int Get(int position, int bit_vector) {
    return data_[bit_vector * kMaxPos + position];
  }

 private:
  RadixIndexToRawIndex() { Initialize(); }

  void Initialize();
  static int ComputeValue(int position, int delete_bit_vector);

  void Set(int position, int bit_vector, int value) {
    data_[bit_vector * kMaxPos + position] = value;
  }

  int data_[kMaxPos << kMaxPos];
};

// This implementation is O(class_size^2) turning a position into a
// permutation but does allows a single position advance for
// Advance(ValueSkip).
template <int kStorageSize>
class ClassPermuterFactorialRadixDeleteTracking final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerStaticStorage<kStorageSize> {
   public:
    using ValueSkip = AdvancerBase::ValueSkip;
    using Base = AdvancerStaticStorage<kStorageSize>;

    explicit Advancer(
        const ClassPermuterFactorialRadixDeleteTracking* permuter);

    void Advance() override;
    void AdvanceDelta(int dist) override;
    void AdvanceSkip(ValueSkip value_skip) override;

   private:
    // Memory based data structure to turn an O(N^2) delete with replacement
    // into an O(N) one.
    RadixIndexToRawIndex<kStorageSize>* radix_index_to_raw_index_;
  };

  explicit ClassPermuterFactorialRadixDeleteTracking(int permutation_size,
                                                     int class_int)
      : ClassPermuter(permutation_size, class_int) {}

  ClassPermuterFactorialRadixDeleteTracking(
      ClassPermuterFactorialRadixDeleteTracking&&) = default;
  ClassPermuterFactorialRadixDeleteTracking& operator=(
      ClassPermuterFactorialRadixDeleteTracking&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this));
  }
};

class MakeClassPermuterFactorialRadixDeleteTracking {
 public:
  std::unique_ptr<ClassPermuter> operator()(int permutation_size = 0,
                                            int class_int = 0);
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H
