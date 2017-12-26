#include <iostream>
#include <memory>
#include <vector>

#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solver.h"

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Solver solver;
  
  SetupProblem(&solver);

  puzzle::Solution got = solver.Solve();
  puzzle::Solution expect = ProblemSolution(solver);
  
  EXPECT_EQ(got, expect);
}

static void BM_Solver(benchmark::State& state) {
  puzzle::Solver solver;
  SetupProblem(&solver);

  for (auto _ : state) {
    solver.Solve();
  }
}

BENCHMARK(BM_Solver);

