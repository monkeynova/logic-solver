#include <iostream>

#include "absl/strings/substitute.h"
#include "benchmark/benchmark.h"
#include "puzzle/class_permuter.h"

using SteinhausJohnsonTrotter = puzzle::internal::ClassPermuterImpl<
    puzzle::internal::ClassPermuterType::kSteinhausJohnsonTrotter>;
using FactorialRadix = puzzle::internal::ClassPermuterImpl<
    puzzle::internal::ClassPermuterType::kFactorialRadix>;

template <typename PermuterType, int depth>
static void BM_Permuter(benchmark::State& state) {
  for (auto _ : state) {
    puzzle::IntRangeDescriptor d(1, depth);
    PermuterType p(&d);
    for (auto it = p.begin(); it != p.end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_Permuter, SteinhausJohnsonTrotter, 3);
BENCHMARK_TEMPLATE(BM_Permuter, SteinhausJohnsonTrotter, 5);
BENCHMARK_TEMPLATE(BM_Permuter, SteinhausJohnsonTrotter, 7);
BENCHMARK_TEMPLATE(BM_Permuter, SteinhausJohnsonTrotter, 9);

BENCHMARK_TEMPLATE(BM_Permuter, FactorialRadix, 3);
BENCHMARK_TEMPLATE(BM_Permuter, FactorialRadix, 5);
BENCHMARK_TEMPLATE(BM_Permuter, FactorialRadix, 7);
BENCHMARK_TEMPLATE(BM_Permuter, FactorialRadix, 9);

template <typename PermuterType, int depth, int crop_column, int crop_value>
static void BM_PermuterActiveSet1InN(benchmark::State& state) {
  puzzle::ActiveSet set;

  // Build ActiveSet.
  puzzle::IntRangeDescriptor d(1, depth);
  PermuterType p(&d);
  for (auto it = p.begin(); it != p.end(); ++it) {
    set.Add((*it)[crop_column] == crop_value);
  }
  set.DoneAdding();
  p.set_active_set(std::move(set));
  state.SetLabel(absl::Substitute("{$0: C($1)=$2}: $3", depth, crop_column,
                                  crop_value, p.Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p.begin(); it != p.end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, SteinhausJohnsonTrotter, 9, 5, 5);

BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 7, 0, 0);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 7, 0, 8);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 7, 0, 7);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 7, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 7, 3, 3);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 9, 0, 9);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 9, 0, 1);
BENCHMARK_TEMPLATE(BM_PermuterActiveSet1InN, FactorialRadix, 9, 5, 5);

template <typename PermuterType, int depth, int every_n>
static void BM_PermuterActiveSetSelectivity(benchmark::State& state) {
  puzzle::ActiveSet set;

  // Build ActiveSet.
  puzzle::IntRangeDescriptor d(1, depth);
  PermuterType p(&d);
  int i = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    set.Add(i % every_n == 0);
    ++i;
  }
  set.DoneAdding();
  p.set_active_set(std::move(set));
  state.SetLabel(
      absl::Substitute("{$0: S=$1}: $2", depth, every_n, p.Selectivity()));

  // Now benchmark with 'set'.
  for (auto _ : state) {
    for (auto it = p.begin(); it != p.end(); ++it) /* no-op */
      ;
  }
}

BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, SteinhausJohnsonTrotter, 7,
                   1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, SteinhausJohnsonTrotter, 7,
                   1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, SteinhausJohnsonTrotter, 9,
                   1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, SteinhausJohnsonTrotter, 9,
                   1000000);

BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, FactorialRadix, 7, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, FactorialRadix, 7, 1000000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, FactorialRadix, 9, 1000);
BENCHMARK_TEMPLATE(BM_PermuterActiveSetSelectivity, FactorialRadix, 9, 1000000);
