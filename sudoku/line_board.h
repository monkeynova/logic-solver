#ifndef SUDOKU_LINE_BOARD_H
#define SUDOKU_LINE_BOARD_H

#include "sudoku/base.h"

namespace sudoku {

class LineBoard : public Base {
 public:
  static std::unique_ptr<LineBoard> Create(absl::string_view line) {
    if (line.size() != 81) {
      LOG(ERROR) << "line length (" << line.size() << ") != 81";
      return nullptr;
    }
    const char* data = line.data();
    Board b;
    b.resize(9);
    for (int row = 0; row < 9; ++row) {
      b[row].resize(9);
      for (int col = 0; col < 9; ++col) {
        if (*data == '.') {
          b[row][col] = -1;
        } else {
          if (*data < '0' || *data > '9') {
            LOG(ERROR) << "Bad input: \"" << *data << "\"";
            return nullptr;
          } else {
            b[row][col] = *data - '0';
          }
        }
        ++data;
      }
    }

    return std::unique_ptr<LineBoard>(new LineBoard(std::move(b)));
  }

  static std::string AsString(const ::puzzle::Solution& solution) {
    DCHECK(solution.IsValid());
    std::string ret;
    ret.resize(81);
    char* out = const_cast<char*>(ret.data());
    for (int row = 0; row < 9; ++row) {
      for (int col = 0; col < 9; ++col) {
	*out = solution.Id(row).Class(col) + '0';
	++out;
      }
    }
    return ret;
  }

  Board GetInstanceBoard() const override { return board_; }
  Board GetSolutionBoard() const override { return board_; }

 private:
  explicit LineBoard(Board board) : board_(std::move(board)) {}

  Board board_;
};

}  // namespace sudoku

#endif  // SUDOKU_LINE_BOARD_H
