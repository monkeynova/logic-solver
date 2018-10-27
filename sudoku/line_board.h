#ifndef SUDOKU_LINE_BOARD_H
#define SUDOKU_LINE_BOARD_H

#include <string>

#include "sudoku/base.h"

namespace sudoku {

class LineBoard : public Base {
 private:
  class PassKey {};

 public:
  explicit LineBoard(Board board, PassKey) : board_(std::move(board)) {}

  static std::unique_ptr<LineBoard> Create(absl::string_view line) {
    Board b = ToBoard(line);
    if (b.empty()) return nullptr;
    return absl::make_unique<LineBoard>(std::move(b), PassKey());
  }

  static std::string ToString(const ::puzzle::Solution& solution);
  static Board ToBoard(absl::string_view line);

  Board GetInstanceBoard() const override { return board_; }
  Board GetSolutionBoard() const override { return board_; }

 private:
  Board board_;
};

}  // namespace sudoku

#endif  // SUDOKU_LINE_BOARD_H
