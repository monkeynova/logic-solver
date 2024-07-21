#include <iostream>
#include <memory>

#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "main_lib.h"
#include "sudoku/line_board.h"

class Stats {
 public:
  Stats() = default;

  void Add(int point) {
    max_ = std::max(point, max_);
    min_ = std::min(point, min_);
    ++count_;
    sum_ += point;
  }

  int min() const { return min_; }
  int max() const { return max_; }
  int avg() const { return sum_ / count_; }

  template <typename Sink>
  friend void AbslStringify(Sink& s, const Stats& st) {
    absl::Format(&s, "%d/%d/%d (%d)", st.min(), st.avg(), st.max(), st.count_);
  }

 private:
  int min_ = std::numeric_limits<int>::max();
  int max_ = std::numeric_limits<int>::min();
  int count_ = 0;
  int sum_ = 0;
};

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat("Solves sudoku boards from STDIN. Usage:\n", argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument!" << std::endl
                            << absl::ProgramUsageMessage();

  int exit_code = 0;
  Stats time_stats;

  absl::Time last_flush = absl::Now();

  for (std::string buf; std::getline(std::cin, buf);) {
    LOG(INFO) << "In:  " << buf;
    absl::Time start = absl::Now();
    ::sudoku::LineBoard line_board(buf);

    absl::Status setup_status = line_board.Setup();
    QCHECK(setup_status.ok()) << setup_status;

    absl::StatusOr<::puzzle::OwnedSolution> answer = line_board.Solve();
    QCHECK(answer.ok()) << answer.status();
    if (answer->IsValid()) {
      absl::Time done = absl::Now();
      int solve_ms = (done - start) / absl::Milliseconds(1);
      time_stats.Add(solve_ms);
      if (done - last_flush > absl::Milliseconds(250)) {
        std::cout << absl::StreamFormat("\033[1K\r%v", time_stats)
                  << std::flush;
        last_flush = done;
      }

      LOG(INFO) << "Out: " << ::sudoku::LineBoard::ToString(answer->view())
                << " (" << solve_ms << "ms)";
    } else {
      LOG(ERROR) << "No answer found";
      exit_code = 1;
    }
  }
  std::cout << std::endl;

  return exit_code;
}
