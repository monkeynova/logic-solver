#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "benchmark/benchmark.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sudoku/line_board.h"

ABSL_FLAG(std::string,sudoku_line_board, "",
              "The sudoku problem to solve as a single line");

ABSL_FLAG(std::string,sudoku_line_answer, "",
              "The sudoku problem to solve as a single line");

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<::puzzle::Problem> line_board =
    ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";

  line_board->Setup();
  ::puzzle::Solution answer = line_board->Solve();
  ASSERT_TRUE(answer.IsValid());

  EXPECT_EQ(::sudoku::LineBoard::ToString(answer), absl::GetFlag(FLAGS_sudoku_line_answer));
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
