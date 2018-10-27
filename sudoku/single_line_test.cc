#include <iostream>
#include <memory>
#include <vector>

#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sudoku/line_board.h"

DEFINE_string(sudoku_line_board, "",
	      "The sudoku problem to solve as a single line");

DEFINE_string(sudoku_line_answer, "",
	      "The sudoku problem to solve as a single line");

TEST(Puzzle, RightAnswer) {
  std::unique_ptr<::puzzle::Problem> line_board =
    ::sudoku::LineBoard::Create(FLAGS_sudoku_line_board);
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";

  line_board->Setup();
  ::puzzle::Solution answer = line_board->Solve();
  ASSERT_TRUE(answer.IsValid());

  EXPECT_EQ(::sudoku::LineBoard::ToString(answer), FLAGS_sudoku_line_answer);
}

static void BM_Solver(benchmark::State& state) {
  std::unique_ptr<::puzzle::Problem> line_board =
    ::sudoku::LineBoard::Create(FLAGS_sudoku_line_board);
  ASSERT_TRUE(line_board != nullptr) << "No puzzle found";

  line_board->Setup();
  for (auto _ : state) {
    ::puzzle::Solution answer = line_board->Solve();
    ASSERT_TRUE(answer.IsValid());
    if (!FLAGS_sudoku_line_answer.empty()) {
      EXPECT_EQ(::sudoku::LineBoard::ToString(answer),
		FLAGS_sudoku_line_answer);
    }
  }
}

BENCHMARK(BM_Solver);
