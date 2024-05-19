#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "main_lib.h"
#include "sudoku/line_board.h"

ABSL_FLAG(std::string, sudoku_line_board, "",
          "The sudoku problem to solve as a single line");

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat("Solves sudoku boards from a flag. Usage:\n", argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument!" << std::endl
                            << absl::ProgramUsageMessage();

  CHECK(!absl::GetFlag(FLAGS_sudoku_line_board).empty())
      << "--sudoku_line_board must be set";

  ::sudoku::LineBoard line_board(absl::GetFlag(FLAGS_sudoku_line_board));
  absl::Status setup_status = line_board.Setup();
  QCHECK(setup_status.ok()) << setup_status;

  absl::StatusOr<::puzzle::Solution> answer = line_board.Solve();
  QCHECK(answer.ok()) << answer.status();
  QCHECK(answer->IsValid());

  char answer_buf[82];
  char* out = answer_buf;
  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      *out = answer->Id(row).Class(col) + '0';
      ++out;
    }
  }
  answer_buf[81] = '\0';

  std::cout << "Out: " << answer_buf << std::endl;

  return 0;
}
