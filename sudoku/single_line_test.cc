#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sudoku/line_board.h"

ABSL_FLAG(bool, puzzle_test_unique, true,
          "If true (default), tests validate that the solution found is "
          "unique.");

ABSL_FLAG(std::string, sudoku_line_board, "",
          "The sudoku problem to solve as a single line");

ABSL_FLAG(std::string, sudoku_line_answer, "",
          "The sudoku problem to solve as a single line");

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<::puzzle::Problem> line_board =
      ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";

  absl::Status st = line_board->Setup();
  ASSERT_TRUE(st.ok()) << st;
  absl::StatusOr<::puzzle::Solution> answer = line_board->Solve();
  ASSERT_TRUE(answer.ok()) << answer.status();
  ASSERT_TRUE(answer->IsValid());

  EXPECT_EQ(::sudoku::LineBoard::ToString(*answer),
            absl::GetFlag(FLAGS_sudoku_line_answer));
}

TEST(Puzzle, UniqueAnswer) {
  if (!absl::GetFlag(FLAGS_puzzle_test_unique)) return;

  std::unique_ptr<::puzzle::Problem> line_board =
      ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";
  absl::Status st = line_board->Setup();
  ASSERT_TRUE(st.ok()) << st;

  absl::StatusOr<std::vector<puzzle::Solution>> solutions =
      line_board->AllSolutions(/*limit=*/2);
  ASSERT_TRUE(solutions.ok()) << solutions.status();
  ASSERT_FALSE(solutions->empty());
  ASSERT_EQ(solutions->size(), 1) << "\n0:\n"
                                  << solutions->at(0) << "\n1:\n"
                                  << solutions->at(1);
}

static void BM_Solver(benchmark::State& state) {
  for (auto _ : state) {
    std::unique_ptr<::puzzle::Problem> line_board =
        ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
    CHECK(line_board != nullptr) << "No puzzle found";

    absl::Status st = line_board->Setup();
    CHECK(st.ok()) << st;

    absl::StatusOr<::puzzle::Solution> answer = line_board->Solve();
    CHECK(answer.ok()) << answer.status();
    CHECK(answer->IsValid());
    if (!absl::GetFlag(FLAGS_sudoku_line_answer).empty()) {
      CHECK_EQ(::sudoku::LineBoard::ToString(*answer),
               absl::GetFlag(FLAGS_sudoku_line_answer));
    }
  }
}

BENCHMARK(BM_Solver)->MeasureProcessCPUTime()->UseRealTime();
