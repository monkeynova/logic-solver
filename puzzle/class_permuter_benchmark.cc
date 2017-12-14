#include "puzzle/class_permuter.h"

#include <iostream>

#include "benchmark/benchmark.h"

BENCHMARK_MAIN();

static void BM_Permuter(benchmark::State& state) {
  for (auto _ : state) {
    puzzle::IntRangeDescriptor d(0, state.range(0));
    puzzle::ClassPermuter p(&d);
    for (auto it = p.begin(); it != p.end(); ++it) {
      /* no-op */
    }
  }
}

BENCHMARK(BM_Permuter)->DenseRange(3,9);
