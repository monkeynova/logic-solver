#include <iostream>

#include "absl/strings/substitute.h"
#include "benchmark/benchmark.h"
#include "puzzle/class_permuter.h"
#include "puzzle/class_permuter_factorial_radix.h"
#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

namespace puzzle {

class MakeSteinhausJohnsonTrotter {
 public:
  std::unique_ptr<ClassPermuter> operator()(const Descriptor* d) {
    return MakeClassPermuterSteinhausJohnsonTrotter(d);
  }
};

class MakeFactorialRadix {
 public:
  std::unique_ptr<ClassPermuter> operator()(const Descriptor* d) {
    return MakeClassPermuterFactorialRadix(d);
  }
};

class MakeFactorialRadixDeleteTracking {
 public:
  std::unique_ptr<ClassPermuter> operator()(const Descriptor* d) {
    return MakeClassPermuterFactorialRadixDeleteTracking(d);
  }
};

template <typename MakePermuterType, int depth>
static void BM_Permuter(benchmark::State& state) {
  for (auto _ : state) {
    IntRangeDescriptor d(1, depth);
    auto p = MakePermuterType()(&d);
    for (auto it = p->begin(); it != p->end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_Permuter, MakeSteinhausJohnsonTrotter, 3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeSteinhausJohnsonTrotter, 5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeSteinhausJohnsonTrotter, 7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeSteinhausJohnsonTrotter, 9);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadix, 3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadix, 5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadix, 7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadix, 9);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadixDeleteTracking, 3);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadixDeleteTracking, 5);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadixDeleteTracking, 7);
BENCHMARK_TEMPLATE(BM_Permuter, MakeFactorialRadixDeleteTracking, 9);

template <typename MakePermuterType, int depth, int crop_column, int crop_value>
static void BM_PermuterActiveSet1InN(benchmark::State& state) {
  ActiveSet set;

  // Build ActiveSet.
  IntRangeDescriptor d(1, depth);
  auto p = MakePermuterType()(&d);
  for (auto it = p->begin(); it != p->end(); ++it) {
    set.Add((*it)[crop_column] == crop_value);
  }
  set.DoneAdding();
  p->set_active_set(std::move(set));
  state.SetLabel(absl::Substitute("{$0: C($1)=$2}: $3", depth, crop_column,
                                  crop_value, p->Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p->begin(); it != p->end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 7, 0,
                   0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 7, 0,
                   8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 7, 0,
                   7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 7, 0,
                   1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 7, 3,
                   3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 9, 0,
                   9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 9, 0,
                   1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeSteinhausJohnsonTrotter, 9, 5,
                   5);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadix, 9, 5, 5);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, MakeFactorialRadixDeleteTracking,
                   9, 5, 5);

template <typename MakePermuterType, int depth, int every_n>
static void BM_PermuterActiveSetSelectivity(benchmark::State& state) {
  ActiveSet set;

  // Build ActiveSet.
  IntRangeDescriptor d(1, depth);
  auto p = MakePermuterType()(&d);
  int i = 0;
  for (auto it = p->begin(); it != p->end(); ++it) {
    set.Add(i % every_n == 0);
    ++i;
  }
  set.DoneAdding();
  p->set_active_set(std::move(set));
  state.SetLabel(
      absl::Substitute("{$0: S=$1}: $2", depth, every_n, p->Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p->begin(); it != p->end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeSteinhausJohnsonTrotter,
                   7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeSteinhausJohnsonTrotter,
                   7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeSteinhausJohnsonTrotter,
                   9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeSteinhausJohnsonTrotter,
                   9, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeFactorialRadix, 7,
                   1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeFactorialRadix, 7,
                   1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeFactorialRadix, 9,
                   1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, MakeFactorialRadix, 9,
                   1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeFactorialRadixDeleteTracking, 7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeFactorialRadixDeleteTracking, 7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeFactorialRadixDeleteTracking, 9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity,
                   MakeFactorialRadixDeleteTracking, 9, 1000000);

}  // namespace puzzle
