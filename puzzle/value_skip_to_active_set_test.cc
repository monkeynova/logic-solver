#include "puzzle/value_skip_to_active_set.h"

#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/class_permuter/factorial_radix.h"
#include "puzzle/class_permuter/factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter/factory.h"
#include "puzzle/class_permuter/steinhaus_johnson_trotter.h"

namespace puzzle {

TEST(ValueSkipToActiveSet, FiveElements) {
  IntRangeDescriptor d(5);
  std::unique_ptr<ClassPermuter> permuter = MakeClassPermuter(&d);
  ValueSkipToActiveSet vs2as(permuter.get());

  for (int position : {0, 1, 2, 3, 4}) {
    for (int value : {0, 1, 2, 3, 4}) {
      ActiveSet as = vs2as.value_skip_set(position, value);
      int loops = 0;
      for (auto it = permuter->begin().WithActiveSet(as); it != permuter->end();
           ++it) {
        EXPECT_NE((*it)[position], value)
            << "Position: " << position << "; iteration: " << it.position();
        ++loops;
      }
      EXPECT_EQ(loops, (5 - 1) * 4 * 3 * 2 * 1) << "Value: " << value;
    }
    for (int value : {-1, 5}) {
      ActiveSet as = vs2as.value_skip_set(position, value);
      int loops = 0;
      for (auto it = permuter->begin().WithActiveSet(as); it != permuter->end();
           ++it) {
        EXPECT_NE((*it)[position], value)
            << "Position: " << position << "; iteration: " << it.position();
        ++loops;
      }
      EXPECT_EQ(loops, 5 * 4 * 3 * 2 * 1) << "Value: " << value;
    }
  }
}

template <typename MakePermuterType>
static void BM_ValueSkipToActiveSet(benchmark::State& state) {
  int depth = state.range(0);
  IntRangeDescriptor d(depth);
  std::unique_ptr<ClassPermuter> permuter = MakeClassPermuter(&d);
  for (auto _ : state) {
    ValueSkipToActiveSet vs2as(permuter.get());
  }
}

BENCHMARK_TEMPLATE(BM_ValueSkipToActiveSet,
                   MakeClassPermuterSteinhausJohnsonTrotter)
    ->Arg(3)
    ->Arg(9);
BENCHMARK_TEMPLATE(BM_ValueSkipToActiveSet, MakeClassPermuterFactorialRadix)
    ->Arg(3)
    ->Arg(9);
BENCHMARK_TEMPLATE(BM_ValueSkipToActiveSet,
                   MakeClassPermuterFactorialRadixDeleteTracking)
    ->Arg(3)
    ->Arg(9);

}  // namespace puzzle
