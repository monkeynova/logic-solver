#include <iostream>
#include <memory>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "sudoku/line_board.h"

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
      std::cout << "Out: " << ::sudoku::LineBoard::ToString(answer)
                << std::endl;
    } else {
      LOG(ERROR) << "No answer found";
      exit_code = 1;
    }
  }

  return exit_code;
}
