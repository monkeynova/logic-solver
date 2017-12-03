#include "puzzle/class_permuter.h"

#include <iostream>

#include "benchmark/benchmark.h"

BENCHMARK_MAIN();

static void BM_Permuter(benchmark::State& state) {
  for (auto _ : state) {
    Puzzle::IntRangeDescriptor d(0, state.range(0));
    Puzzle::ClassPermuter p(&d);
    for (auto it = p.begin(); it != p.end(); ++it) {
      /* no-op */
    }
  }
}

BENCHMARK(BM_Permuter)->DenseRange(3,9);
