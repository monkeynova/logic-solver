#include <iostream>
#include <memory>
#include <vector>

#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solver.h"

DEFINE_string(benchmarks, "", "...");

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Solver solver;
  
  SetupProblem(&solver);

  puzzle::Solution got = solver.Solve();
  puzzle::Solution expect = ProblemSolution(solver);
  
  EXPECT_EQ(got, expect) ;
}

static void BM_Solver(benchmark::State& state) {
  puzzle::Solver solver;
  SetupProblem(&solver);

  for (auto _ : state) {
    solver.Solve();
  }
}

BENCHMARK(BM_Solver);

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::benchmark::Initialize(&argc, argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  if (!FLAGS_benchmarks.empty()) {
    ::benchmark::RunSpecifiedBenchmarks();
  }
  return RUN_ALL_TESTS();
}
