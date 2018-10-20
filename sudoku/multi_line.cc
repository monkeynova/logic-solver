#include <iostream>
#include <memory>

#include "gflags/gflags.h"
#include "glog/logging.h"
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
  
  Board GetInstanceBoard() const override { return board_; }
  Board GetSolutionBoard() const override { return board_; }

 private:
  explicit LineBoard(Board board) : board_(std::move(board)) {}
  
  Board board_;
};

}  // namespace sudoku

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);

  int exit_code = 0;
  
  for (std::string buf; std::getline(std::cin, buf); ) {
    std::cout << "In:  " << buf << std::endl;
    std::unique_ptr<::puzzle::Problem> line_board =
        ::sudoku::LineBoard::Create(buf);
    CHECK(line_board != nullptr) << "No puzzle found";

    line_board->Setup();

    ::puzzle::Solution answer = line_board->Solve();
    if (answer.IsValid()) {
      char answer_buf[82];
      char* out = answer_buf;
      for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
	  *out = answer.Id(row).Class(col) + '0';
	  ++out;
	}
      }
      answer_buf[81] = '\0';

      std::cout << "Out: " << answer_buf << std::endl;
    } else {
      LOG(ERROR) << "No answer found";
      exit_code = 1;
    }
  }

  return exit_code;
}
