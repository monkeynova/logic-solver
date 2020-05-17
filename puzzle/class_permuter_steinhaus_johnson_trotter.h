#ifndef PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H
#define PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
class ClassPermuterSteinhausJohnsonTrotter final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerBase {
   public:
    Advancer(const ClassPermuterSteinhausJohnsonTrotter* permuter,
             ActiveSet active_set);

    std::unique_ptr<AdvancerBase> Clone() const override {
      return absl::make_unique<Advancer>(*this);
    }

    void Advance() override;
    void Advance(int dist) override;

   private:
    StorageVector index_;
    StorageVector direction_;
    int next_from_;
  };

  explicit ClassPermuterSteinhausJohnsonTrotter(const Descriptor* d = nullptr,
                                                int class_int = 0)
      : ClassPermuter(d, class_int) {}

  ClassPermuterSteinhausJohnsonTrotter(ClassPermuterSteinhausJohnsonTrotter&&) =
      default;
  ClassPermuterSteinhausJohnsonTrotter& operator=(
      ClassPermuterSteinhausJohnsonTrotter&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_STEINHAUS_JOHNSON_TROTTER_H
