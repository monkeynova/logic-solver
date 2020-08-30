#include <iostream>
#include <memory>

#include "absl/time/time.h"
#include "main_lib.h"
#include "sudoku/line_board.h"

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  int exit_code = 0;

  for (std::string buf; std::getline(std::cin, buf);) {
    std::cout << "In:  " << buf << std::endl;
    absl::Time start = absl::Now();
    std::unique_ptr<::puzzle::Problem> line_board =
        ::sudoku::LineBoard::Create(buf);
    CHECK(line_board != nullptr) << "No puzzle found";

    line_board->Setup();

    ::puzzle::Solution answer = line_board->Solve();
    if (answer.IsValid()) {
      absl::Time done = absl::Now();
      std::cout << "Out: " << ::sudoku::LineBoard::ToString(answer) << " ("
                << 1e-3 * ((done - start) / absl::Milliseconds(1)) << "s)"
                << std::endl;
    } else {
      LOG(ERROR) << "No answer found";
      exit_code = 1;
    }
  }

  return exit_code;
}
