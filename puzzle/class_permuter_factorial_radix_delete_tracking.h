#ifndef PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H
#define PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H

#include "puzzle/class_permuter.h"

namespace puzzle {

// Contains a map from a radix index position and a bit vector marked with
// previously selected values from index_ in a permutation to the index for
// the correspondingly selected value in index_.
// This is a lookup-table for the function ComputeRadixIndexToRawIndex which
// is called in the innermost loop.
using RadixIndexToRawIndex = std::vector<std::vector<int>>;

// This implementation is O(class_size^2) turning a position into a
// permutation but does allows a single position advance for
// Advance(ValueSkip).
class ClassPermuterFactorialRadixDeleteTracking final : public ClassPermuter {
 public:
  class Advancer final : public AdvancerBase {
   public:
    Advancer(const ClassPermuterFactorialRadixDeleteTracking* permuter,
             ActiveSet active_set);

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

  explicit ClassPermuterFactorialRadixDeleteTracking(
      const Descriptor* d = nullptr, int class_int = 0)
      : ClassPermuter(d, class_int) {}

  ClassPermuterFactorialRadixDeleteTracking(
      ClassPermuterFactorialRadixDeleteTracking&&) = default;
  ClassPermuterFactorialRadixDeleteTracking& operator=(
      ClassPermuterFactorialRadixDeleteTracking&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, active_set()));
  }
  iterator begin(ActiveSet active_set) const override {
    return iterator(absl::make_unique<Advancer>(this, std::move(active_set)));
  }
};

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_FACTORIAL_RADIX_DELETE_TRACKING_H
