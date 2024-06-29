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

ABSL_DECLARE_FLAG(std::string, puzzle_solution_permuter);
ABSL_DECLARE_FLAG(bool, puzzle_prune_class_iterator);
ABSL_DECLARE_FLAG(bool, puzzle_prune_reorder_classes);

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  absl::Status setup_status = problem->Setup();
  ASSERT_TRUE(setup_status.ok()) << setup_status;

  absl::StatusOr<puzzle::Solution> got = problem->Solve();
  ASSERT_TRUE(got.ok()) << got.status();
  absl::StatusOr<puzzle::Solution> expect = problem->GetSolution();
  ASSERT_TRUE(expect.ok()) << expect.status();

  EXPECT_EQ(*got, *expect);
}

TEST(Puzzle, UniqueAnswer) {
  if (!absl::GetFlag(FLAGS_puzzle_test_unique)) return;

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  absl::Status setup_status = problem->Setup();
  ASSERT_TRUE(setup_status.ok()) << setup_status;

  absl::StatusOr<std::vector<puzzle::Solution>> solutions =
      problem->AllSolutions(/*limit=*/2);
  ASSERT_TRUE(solutions.ok()) << solutions.status();
  ASSERT_FALSE(solutions->empty());
  ASSERT_EQ(solutions->size(), 1)
      << "\n0: " << solutions->at(0) << "\n1: " << solutions->at(1);
}

static void BM_Solver(benchmark::State& state) {
  struct Args {
    std::string permuter;
    bool prune;
    bool reorder;
    std::string label;
  };
  static const std::array<Args, 5> all_args = {
    Args{.permuter = "brute", .prune = false, .reorder = false, .label = "brute"},
    Args{.permuter = "allowonly", .prune = false, .reorder = false, .label = "allowonly"},
    Args{.permuter = "filtered", .prune = false, .reorder = false, .label = "filters"},
    Args{.permuter = "filtered", .prune = true, .reorder = false, .label = "filters+prune"},
    Args{.permuter = "filtered", .prune = true, .reorder = true, .label = "filters+prune+reorder"},
  };
  const Args& args = all_args[state.range(0)];
  absl::SetFlag(&FLAGS_puzzle_solution_permuter, args.permuter);
  absl::SetFlag(&FLAGS_puzzle_prune_class_iterator, args.prune);
  absl::SetFlag(&FLAGS_puzzle_prune_reorder_classes, args.reorder);
  state.SetLabel(args.label);

  for (auto _ : state) {
    std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
    CHECK(problem->Setup().ok());
    CHECK(problem->Solve().ok());
  }
}

BENCHMARK(BM_Solver)->DenseRange(0, 4)->MeasureProcessCPUTime()->UseRealTime();
