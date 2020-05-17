#ifndef PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H
#define PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// Treats the permutation index as a factorial radix number
// ({0..8} * 8! + {0..7} * 7! + ... {0..1} * 1! + {0} * 0!).
// This implementation is O(class_size) turning a position into a permutation
// but does not allow seeking to a position for Advance(ValueSkip).
class ClassPermuterFactorialRadix final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerBase {
   public:
    Advancer(const ClassPermuterFactorialRadix* permuter, ActiveSet active_set);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void Advance(int dist) override;

   private:
    StorageVector index_;
  };

  explicit ClassPermuterFactorialRadix(const Descriptor* d = nullptr,
                                       int class_int = 0)
      : ClassPermuter(d, class_int) {}

  ClassPermuterFactorialRadix(ClassPermuterFactorialRadix&&) = default;
  ClassPermuterFactorialRadix& operator=(ClassPermuterFactorialRadix&&) =
      default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_H
