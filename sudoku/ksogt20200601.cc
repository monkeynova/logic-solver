#include "sudoku/greater_than_sudoku.h"

namespace sudoku {

// https://www.killersudokuonline.com/play.html?puzzle=GD3ro4xz5266&year=2020
class KSOGT20200601 : public GreaterThanSudoku {
 public:
  std::vector<std::pair<Box, Box>> GetComparisons() const override {
    return {// Upper three rows.
            {{1, 0}, {0, 0}},
            {{0, 1}, {1, 1}},
            {{0, 4}, {0, 3}},
            {{0, 4}, {0, 5}},
            {{0, 4}, {1, 4}},
            {{1, 7}, {0, 7}},
            {{0, 7}, {0, 8}},
            {{1, 0}, {2, 0}},
            {{2, 1}, {1, 1}},
            {{1, 1}, {1, 2}},
            {{1, 2}, {2, 2}},
            {{2, 3}, {1, 3}},
            {{1, 3}, {1, 4}},
            {{1, 4}, {2, 4}},
            {{1, 4}, {1, 5}},
            {{2, 5}, {1, 5}},
            {{1, 8}, {2, 8}},
            {{2, 6}, {2, 7}},
            // Middle three rows.
            {{3, 0}, {4, 0}},
            {{3, 1}, {3, 0}},
            {{4, 1}, {3, 1}},
            {{3, 2}, {3, 1}},
            {{3, 2}, {4, 2}},
            {{3, 4}, {3, 3}},
            {{3, 4}, {4, 4}},
            {{3, 5}, {4, 5}},
            {{4, 6}, {3, 6}},
            {{3, 6}, {3, 7}},
            {{3, 7}, {4, 7}},
            {{3, 7}, {3, 8}},
            {{4, 1}, {4, 0}},
            {{4, 1}, {5, 1}},
            {{4, 4}, {5, 4}},
            {{5, 8}, {4, 8}},
            {{5, 1}, {5, 0}},
            {{5, 3}, {5, 4}},
            {{5, 4}, {5, 5}},
            {{5, 7}, {5, 8}},
            // Lower three rows.
            {{6, 0}, {7, 0}},
            {{6, 2}, {7, 2}},
            {{6, 3}, {6, 4}},
            {{6, 5}, {6, 4}},
            {{7, 5}, {6, 5}},
            {{7, 6}, {6, 6}},
            {{6, 6}, {6, 7}},
            {{6, 7}, {6, 8}},
            {{8, 1}, {7, 1}},
            {{7, 1}, {7, 2}},
            {{8, 3}, {7, 3}},
            {{7, 5}, {7, 4}},
            {{7, 6}, {7, 7}},
            {{7, 7}, {7, 8}},
            {{7, 8}, {8, 8}},
            {{8, 1}, {8, 2}},
            {{8, 3}, {8, 4}},
            {{8, 4}, {8, 5}},
            {{8, 7}, {8, 6}},
            {{8, 8}, {8, 7}}};
  }

  absl::StatusOr<Sudoku::Board> GetSolutionBoard() const override {
    return Board{{6, 8, 9, 4, 7, 2, 3, 5, 1}, {7, 4, 3, 6, 5, 1, 2, 8, 9},
                 {2, 5, 1, 8, 3, 9, 7, 4, 6}, {5, 6, 8, 1, 9, 7, 4, 3, 2},
                 {3, 9, 4, 2, 8, 6, 5, 1, 7}, {1, 2, 7, 5, 4, 3, 6, 9, 8},
                 {9, 1, 6, 3, 2, 5, 8, 7, 4}, {4, 3, 2, 7, 1, 8, 9, 6, 5},
                 {8, 7, 5, 9, 6, 4, 1, 2, 3}};
  }
};

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::KSOGT20200601);
