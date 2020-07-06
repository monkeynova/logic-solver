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

  line_board->Setup();
  ::puzzle::Solution answer = line_board->Solve();
  ASSERT_TRUE(answer.IsValid());

  EXPECT_EQ(::sudoku::LineBoard::ToString(answer),
            absl::GetFlag(FLAGS_sudoku_line_answer));
}

TEST(Puzzle, UniqueAnswer) {
  if (!absl::GetFlag(FLAGS_puzzle_test_unique)) return;

  std::unique_ptr<::puzzle::Problem> line_board =
      ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";
  line_board->Setup();

  std::vector<puzzle::Solution> solutions =
      line_board->AllSolutions(/*limit=*/2);
  ASSERT_FALSE(solutions.empty());
  ASSERT_EQ(solutions.size(), 1) << "\n0:\n"
                                 << solutions[0].DebugString() << "\n1:\n"
                                 << solutions[1].DebugString();
}

static void BM_Solver(benchmark::State& state) {
  std::unique_ptr<::puzzle::Problem> line_board =
      ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";

  line_board->Setup();
  for (auto _ : state) {
    ::puzzle::Solution answer = line_board->Solve();
    ASSERT_TRUE(answer.IsValid());
    if (!absl::GetFlag(FLAGS_sudoku_line_answer).empty()) {
      EXPECT_EQ(::sudoku::LineBoard::ToString(answer),
                absl::GetFlag(FLAGS_sudoku_line_answer));
    }
  }
}

BENCHMARK(BM_Solver);
