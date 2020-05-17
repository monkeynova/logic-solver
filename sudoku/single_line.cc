#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "glog/logging.h"
#include "sudoku/line_board.h"

ABSL_FLAG(std::string,sudoku_line_board, "",
              "The sudoku problem to solve as a single line");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);

  CHECK(!absl::GetFlag(FLAGS_sudoku_line_board).empty()) << "--sudoku_line_board must be set";

  std::unique_ptr<::puzzle::Problem> line_board =
    ::sudoku::LineBoard::Create(absl::GetFlag(FLAGS_sudoku_line_board));
  CHECK(line_board != nullptr) << "No puzzle found";

  line_board->Setup();

  ::puzzle::Solution answer = line_board->Solve();
  CHECK(answer.IsValid());

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

  return 0;
}
