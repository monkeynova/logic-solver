#include <iostream>
#include <memory>
#include <vector>

#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solver.h"

DECLARE_bool(puzzle_brute_force);
DECLARE_bool(puzzle_prune_class_iterator);
DECLARE_bool(puzzle_prune_reorder_classes);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Solver solver;
  
  SetupProblem(&solver);

  puzzle::Solution got = solver.Solve();
  puzzle::Solution expect = ProblemSolution(solver);
  
  EXPECT_EQ(got, expect);
}

template <bool brute, bool prune, bool reorder>
static void BM_Solver(benchmark::State& state) {
  puzzle::Solver solver;
  SetupProblem(&solver);

  std::vector<std::string> label;
  if (brute) {
    FLAGS_puzzle_brute_force = true;
    label.push_back("brute");
  }
  if (prune) {
    FLAGS_puzzle_prune_class_iterator = true;
    label.push_back("prune");
  }
  if (reorder) {
    FLAGS_puzzle_prune_reorder_classes = true;
    label.push_back("reorder");
  }
  state.SetLabel(absl::StrJoin(label, " "));

  for (auto _ : state) {
    solver.Solve();
  }
}

BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/false,
		   /*reorder=*/false);
BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/true, /*prune=*/false,
		   /*reorder=*/false);
BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/true,
		   /*reorder=*/false);
BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/true,
		   /*reorder=*/true);

