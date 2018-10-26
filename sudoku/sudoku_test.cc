#include <iostream>
#include <memory>
#include <vector>

#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/problem.h"

DECLARE_string(sudoku_problem_setup);
DECLARE_bool(puzzle_prune_pair_class_iterators);
DECLARE_bool(puzzle_prune_pair_class_iterators_mode_pair);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Problem* problem = puzzle::Problem::GetInstance();
  problem->Setup();

  puzzle::Solution got = problem->Solve();
  ASSERT_TRUE(got.IsValid());

  EXPECT_EQ(got, problem->GetSolution());
}

static void SetFlag(bool val, absl::string_view label, bool* flag,
                    std::vector<std::string>* labels) {
  *flag = val;
  labels->push_back(val ? std::string(label) : absl::StrCat("no", label));
}

template <bool pair_iterators, bool mode_pair>
static void BM_Solver(benchmark::State& state) {
  puzzle::Problem* problem = puzzle::Problem::GetInstance();
  problem->Setup();

  puzzle::Solution expect = problem->GetSolution();

  std::vector<std::string> labels;
  SetFlag(pair_iterators, "pair_iterators",
          &FLAGS_puzzle_prune_pair_class_iterators, &labels);
  SetFlag(mode_pair, "mode_pair",
          &FLAGS_puzzle_prune_pair_class_iterators_mode_pair, &labels);
  state.SetLabel(absl::StrJoin(labels, " "));

  for (auto _ : state) {
    puzzle::Solution got = problem->Solve();
    EXPECT_EQ(got, expect);
  }
}

BENCHMARK_TEMPLATE(BM_Solver,
                   /*pair_iterators=*/true,
                   /*mode_pair=*/false);

BENCHMARK_TEMPLATE(BM_Solver,
                   /*pair_iterators=*/true,
                   /*mode_pair=*/true);

