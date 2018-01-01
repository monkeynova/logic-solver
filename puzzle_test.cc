#include <iostream>
#include <memory>
#include <vector>

#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/problem.h"

DECLARE_bool(puzzle_brute_force);
DECLARE_bool(puzzle_prune_class_iterator);
DECLARE_bool(puzzle_prune_reorder_classes);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Problem* problem = puzzle::Problem::GetInstance();
   problem->Setup();

  puzzle::Solution got = problem->Solve();
  puzzle::Solution expect = problem->Solution();
  
  EXPECT_EQ(got, expect);
}

template <bool brute, bool noprune, bool noreorder>
static void BM_Solver(benchmark::State& state) {
  puzzle::Problem* problem = puzzle::Problem::GetInstance();
  problem->Setup();

  std::vector<std::string> label;
  if (brute) {
    FLAGS_puzzle_brute_force = true;
    label.push_back("brute");
  }
  if (noprune) {
    FLAGS_puzzle_prune_class_iterator = false;
    label.push_back("noprune");
  }
  if (noreorder) {
    FLAGS_puzzle_prune_reorder_classes = false;
    label.push_back("noreorder");
  }
  state.SetLabel(absl::StrJoin(label, " "));

  for (auto _ : state) {
    problem->Solve();
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

