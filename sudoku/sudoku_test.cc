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

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  absl::Status setup_status = problem->Setup();
  ASSERT_TRUE(setup_status.ok()) << setup_status;

  absl::StatusOr<puzzle::OwnedSolution> got = problem->Solve();
  ASSERT_TRUE(got.ok()) << got.status();
  ASSERT_TRUE(got->IsValid());
  absl::StatusOr<puzzle::OwnedSolution> expected = problem->GetSolution();
  ASSERT_TRUE(expected.ok()) << expected.status();

  EXPECT_EQ(*got, *expected);
}

TEST(Puzzle, UniqueAnswer) {
  if (!absl::GetFlag(FLAGS_puzzle_test_unique)) return;

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  absl::Status setup_status = problem->Setup();
  ASSERT_TRUE(setup_status.ok()) << setup_status;

  absl::StatusOr<std::vector<puzzle::OwnedSolution>> solutions =
      problem->AllSolutions(/*limit=*/2);
  ASSERT_TRUE(solutions.ok()) << solutions.status();
  ASSERT_FALSE(solutions->empty());
  ASSERT_EQ(solutions->size(), 1) << "\n0:\n"
                                  << solutions->at(0) << "\n1:\n"
                                  << solutions->at(1);
}

static void SetFlag(bool val, absl::string_view label, absl::Flag<bool>* flag,
                    std::vector<std::string>* labels) {
  absl::SetFlag(flag, val);
  labels->push_back(val ? std::string(label) : absl::StrCat("no", label));
}

template <bool pair_iterators, bool mode_pair>
static void BM_Solver(benchmark::State& state) {
  for (auto _ : state) {
    std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
    absl::Status st = problem->Setup();
    CHECK(st.ok()) << st;

    absl::StatusOr<puzzle::OwnedSolution> expect = problem->GetSolution();
    CHECK(expect.ok()) << expect.status();

    std::vector<std::string> labels;
    SetFlag(pair_iterators, "pair_iterators",
            &FLAGS_puzzle_prune_pair_class_iterators, &labels);
    SetFlag(mode_pair, "mode_pair",
            &FLAGS_puzzle_prune_pair_class_iterators_mode_pair, &labels);
    state.SetLabel(absl::StrJoin(labels, " "));

    absl::StatusOr<puzzle::OwnedSolution> got = problem->Solve();
    CHECK(got.ok()) << got.status();
    CHECK(*got == *expect);
  }
}

BENCHMARK_TEMPLATE(BM_Solver,
                   /*pair_iterators=*/true,
                   /*mode_pair=*/false)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_Solver,
                   /*pair_iterators=*/true,
                   /*mode_pair=*/true)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
