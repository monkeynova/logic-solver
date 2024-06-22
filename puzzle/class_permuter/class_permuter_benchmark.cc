#include <iostream>

#include "absl/strings/substitute.h"
#include "benchmark/benchmark.h"
#include "puzzle/class_permuter/class_permuter.h"
#include "puzzle/class_permuter/factorial_radix.h"
#include "puzzle/class_permuter/factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter/steinhaus_johnson_trotter.h"

namespace puzzle {

template <typename MakeClassPermuterPermuterType, int depth>
static void BM_Permuter(benchmark::State& state) {
  auto p = MakeClassPermuterPermuterType()(depth);
  for (auto _ : state) {
    for (auto it = p->begin(); it != p->end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterSteinhausJohnsonTrotter, 3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterSteinhausJohnsonTrotter, 5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterSteinhausJohnsonTrotter, 7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterSteinhausJohnsonTrotter, 9);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadix, 3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadix, 5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadix, 7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadix, 9);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadixDeleteTracking,
                   3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadixDeleteTracking,
                   5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadixDeleteTracking,
                   7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeClassPermuterFactorialRadixDeleteTracking,
                   9);

template <typename MakeClassPermuterPermuterType, int depth, int crop_column,
          int crop_value>
static void BM_PermuterActiveSet1InN(benchmark::State& state) {
  // Build ActiveSet.
  auto p = MakeClassPermuterPermuterType()(depth);

  ActiveSet::Builder builder(p->permutation_count());
  for (auto it = p->begin(); it != p->end(); ++it) {
    builder.Add((*it)[crop_column] == crop_value);
  }
  ActiveSet set = builder.DoneAdding();
  state.SetLabel(absl::Substitute("{$0: C($1)=$2}: $3", depth, crop_column,
                                  crop_value, set.Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p->begin().WithActiveSet(set); it != p->end();
         ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterSteinhausJohnsonTrotter, 9, 5, 5);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 7,
                   0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 7,
                   0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 7,
                   0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 7,
                   0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 7,
                   3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 9,
                   0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 9,
                   0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeClassPermuterFactorialRadix, 9,
                   5, 5);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN,
                   MakeClassPermuterFactorialRadixDeleteTracking, 9, 5, 5);

template <typename MakeClassPermuterPermuterType, int depth, int every_n>
static void BM_PermuterActiveSetSelectivity(benchmark::State& state) {
  // Build ActiveSet.
  auto p = MakeClassPermuterPermuterType()(depth);

  ActiveSet::Builder builder(p->permutation_count());
  int i = 0;
  for (auto it = p->begin(); it != p->end(); ++it) {
    builder.Add(i % every_n == 0);
    ++i;
  }
  ActiveSet set = builder.DoneAdding();
  state.SetLabel(
      absl::Substitute("{$0: S=$1}: $2", depth, every_n, set.Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p->begin().WithActiveSet(set); it != p->end();
         ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterSteinhausJohnsonTrotter, 7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterSteinhausJohnsonTrotter, 9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterSteinhausJohnsonTrotter, 9, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadix, 7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadix, 7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadix, 9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadix, 9, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadixDeleteTracking, 7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadixDeleteTracking, 9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeClassPermuterFactorialRadixDeleteTracking, 9, 1000000);

}  // namespace puzzle
