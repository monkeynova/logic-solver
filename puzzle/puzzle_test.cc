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

ABSL_DECLARE_FLAG(bool, puzzle_brute_force);
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

static void SetFlag(bool val, absl::string_view label, absl::Flag<bool>* flag,
                    std::vector<std::string>* labels) {
  absl::SetFlag(flag, val);
  labels->push_back(val ? std::string(label) : absl::StrCat("no", label));
}

template <bool brute, bool prune, bool reorder>
static void BM_Solver(benchmark::State& state) {
  std::vector<std::string> labels;
  SetFlag(brute, "brute", &FLAGS_puzzle_brute_force, &labels);
  SetFlag(prune, "prune", &FLAGS_puzzle_prune_class_iterator, &labels);
  SetFlag(reorder, "reorder", &FLAGS_puzzle_prune_reorder_classes, &labels);
  state.SetLabel(absl::StrJoin(labels, " "));

  for (auto _ : state) {
    std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
    CHECK(problem->Setup().ok());
    CHECK(problem->Solve().ok());
  }
}

BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/false,
                   /*reorder=*/false)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/true, /*prune=*/false,
                   /*reorder=*/false)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/true,
                   /*reorder=*/false)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_Solver, /*brute=*/false, /*prune=*/true,
                   /*reorder=*/true)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
