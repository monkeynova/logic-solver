#ifndef PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H
#define PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
template <int kStorageSize>
class ClassPermuterSteinhausJohnsonTrotter final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerStaticStorage<kStorageSize> {
   public:
    using Base = AdvancerStaticStorage<kStorageSize>;

    Advancer(const ClassPermuterSteinhausJohnsonTrotter* permuter);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void AdvanceDelta(int dist) override;

   private:
    int index_[kStorageSize];
    int direction_[kStorageSize];
    int next_from_;
  };

  explicit ClassPermuterSteinhausJohnsonTrotter(const Descriptor* d,
                                                int class_int)
      : ClassPermuter(d, class_int) {}

  ClassPermuterSteinhausJohnsonTrotter(ClassPermuterSteinhausJohnsonTrotter&&) =
      default;
  ClassPermuterSteinhausJohnsonTrotter& operator=(
      ClassPermuterSteinhausJohnsonTrotter&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this));
  }
};

class MakeClassPermuterSteinhausJohnsonTrotter {
 public:
  std::unique_ptr<ClassPermuter> operator()(const Descriptor* d = nullptr,
                                            int class_int = 0);
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H
