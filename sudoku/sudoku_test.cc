#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/declare.h"
#include "absl/flags/flag.h"
#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/problem.h"

ABSL_FLAG(bool, puzzle_test_unique, true,
          "If true (default), tests validate that the solution found is "
          "unique.");

ABSL_DECLARE_FLAG(std::string, sudoku_problem_setup);
ABSL_DECLARE_FLAG(bool, puzzle_prune_pair_class_iterators);
ABSL_DECLARE_FLAG(bool, puzzle_prune_pair_class_iterators_mode_pair);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  problem->Setup();

  puzzle::Solution got = problem->Solve();
  ASSERT_TRUE(got.IsValid());

  EXPECT_EQ(got, problem->GetSolution());
}

TEST(Puzzle, UniqueAnswer) {
  if (!absl::GetFlag(FLAGS_puzzle_test_unique)) return;

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  problem->Setup();

  std::vector<puzzle::Solution> solutions = problem->AllSolutions(/*limit=*/2);
  ASSERT_EQ(solutions.size(), 1);
}

static void SetFlag(bool val, absl::string_view label, absl::Flag<bool>* flag,
                    std::vector<std::string>* labels) {
  absl::SetFlag(flag, val);
  labels->push_back(val ? std::string(label) : absl::StrCat("no", label));
}

template <bool pair_iterators, bool mode_pair>
static void BM_Solver(benchmark::State& state) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
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
