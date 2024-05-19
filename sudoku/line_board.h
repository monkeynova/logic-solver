#ifndef SUDOKU_LINE_BOARD_H
#define SUDOKU_LINE_BOARD_H

#include <string>

#include "sudoku/sudoku.h"

namespace sudoku {

class LineBoard : public Sudoku {
 public:
  using Grid<9>::ToString;
  using Grid<9>::ToBoard;

  explicit LineBoard(absl::string_view line) : board_(ToBoard(line)) {}

  absl::StatusOr<Board> GetInstanceBoard() const override { return board_; }
  absl::StatusOr<Board> GetSolutionBoard() const override { return board_; }

 private:
  absl::StatusOr<Board> board_;
};

}  // namespace sudoku

#endif  // SUDOKU_LINE_BOARD_H
