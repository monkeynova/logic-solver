#include "sudoku/line_board.h"

namespace sudoku {

// static
::sudoku::Base::Board LineBoard::ToBoard(absl::string_view line) {
  if (line.size() != 81) {
    LOG(ERROR) << "line length (" << line.size() << ") != 81";
    return Board();
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
          return Board();
        } else {
          b[row][col] = *data - '0';
        }
      }
      ++data;
    }
  }

  return b;
}

// static
std::string LineBoard::ToString(const ::puzzle::Solution& solution) {
  DCHECK(solution.IsValid());
  std::string ret;
  ret.resize(81);
  char* out = const_cast<char*>(ret.data());
  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      // Translate from 0-indexed solution space to 1-indexed sudoku board.
      *out = solution.Id(row).Class(col) + '1';
      ++out;
    }
  }
  return ret;
}

}  // namespace sudoku
