#ifndef PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H
#define PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H

#include "puzzle/class_permuter/class_permuter.h"

namespace puzzle {

// This implementation is O(class_size^2) turning a position into a
// permutation but does allows a single position advance for
// Advance(ValueSkip).
template <int kStorageSize>
class ClassPermuterFactorialRadixDeleteTracking final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerStaticStorage<kStorageSize> {
   public:
    using Base = AdvancerStaticStorage<kStorageSize>;

    explicit Advancer(
        const ClassPermuterFactorialRadixDeleteTracking* permuter);

    void Advance() override;
    void AdvanceDelta(int dist) override;
    void AdvanceSkip(ValueSkip value_skip) override;
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
