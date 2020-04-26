#include <iostream>
#include <memory>
#include <vector>

#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/problem.h"

DECLARE_bool(puzzle_brute_force);
DECLARE_bool(puzzle_prune_class_iterator);
DECLARE_bool(puzzle_prune_reorder_classes);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  problem->Setup();

  puzzle::Solution got = problem->Solve();
  puzzle::Solution expect = problem->GetSolution();

  EXPECT_EQ(got, expect);
}

static void SetFlag(bool val, absl::string_view label, bool* flag,
                    std::vector<std::string>* labels) {
  *flag = val;
  labels->push_back(val ? std::string(label) : absl::StrCat("no", label));
}

template <bool brute, bool prune, bool reorder>
static void BM_Solver(benchmark::State& state) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  problem->Setup();

  std::vector<std::string> labels;
  SetFlag(brute, "brute", &FLAGS_puzzle_brute_force, &labels);
  SetFlag(prune, "prune", &FLAGS_puzzle_prune_class_iterator, &labels);
  SetFlag(reorder, "reorder", &FLAGS_puzzle_prune_reorder_classes, &labels);
  state.SetLabel(absl::StrJoin(labels, " "));

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

