#ifndef PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H
#define PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// Treats the permutation index as a factorial radix number
// ({0..8} * 8! + {0..7} * 7! + ... {0..1} * 1! + {0} * 0!).
// This implementation is O(class_size) turning a position into a permutation
// but does not allow seeking to a position for Advance(ValueSkip).
template <int kStorageSize>
class ClassPermuterFactorialRadix final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerStaticStorage<kStorageSize> {
   public:
    using Base = AdvancerStaticStorage<kStorageSize>;

    explicit Advancer(const ClassPermuterFactorialRadix* permuter);

    void Advance() override;
    void AdvanceDelta(int dist) override;
  };

  explicit ClassPermuterFactorialRadix(int permutation_size, int class_int)
      : ClassPermuter(permutation_size, class_int) {}

  ClassPermuterFactorialRadix(ClassPermuterFactorialRadix&&) = default;
  ClassPermuterFactorialRadix& operator=(ClassPermuterFactorialRadix&&) =
      default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this));
  }
};

class MakeClassPermuterFactorialRadix {
 public:
  std::unique_ptr<ClassPermuter> operator()(int permutation_size = 0,
                                            int class_int = 0);
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H
