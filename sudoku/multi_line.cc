#include <iostream>
#include <memory>

#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "main_lib.h"
#include "sudoku/line_board.h"

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat("Solves sudoku boards from STDIN. Usage:\n", argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument!" << std::endl
                            << absl::ProgramUsageMessage();

  int exit_code = 0;

  for (std::string buf; std::getline(std::cin, buf);) {
    std::cout << "In:  " << buf << std::endl;
    absl::Time start = absl::Now();
    ::sudoku::LineBoard line_board(buf);

    absl::Status setup_status = line_board.Setup();
    QCHECK(setup_status.ok()) << setup_status;

    absl::StatusOr<::puzzle::Solution> answer = line_board.Solve();
    QCHECK(answer.ok()) << answer.status();
    if (answer->IsValid()) {
      absl::Time done = absl::Now();
      std::cout << "Out: " << ::sudoku::LineBoard::ToString(*answer) << " ("
                << 1e-3 * ((done - start) / absl::Milliseconds(1)) << "s)"
                << std::endl;
    } else {
      LOG(ERROR) << "No answer found";
      exit_code = 1;
    }
  }

  return exit_code;
}
