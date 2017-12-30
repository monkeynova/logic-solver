#include <iostream>
#include <memory>
#include <vector>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "puzzle/instance.h"

DEFINE_bool(all, false, "Show all solutions");

extern void SetupProblem(puzzle::Solver* s);

std::string PositionHeader(const puzzle::Solution& s) {
  return absl::StrCat("[position=", s.permutation_position(), "/",
                      s.permutation_count(), " (", s.Completion(), ")]");
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  puzzle::Solver solver;
  
  SetupProblem(&solver);
  
  int exit_code = 1;
  
  if (FLAGS_all) {
    LOG(INFO) << "[AllSolutions]";
    std::vector<puzzle::Solution> all_solutions = solver.AllSolutions();
    exit_code = all_solutions.size() > 0 ? 0 : 1;
    LOG(INFO) << "[" << all_solutions.size() << " solutions]";
    LOG(INFO)
      << absl::StrJoin(
             all_solutions, "\n",
             [](std::string* out, const puzzle::Solution& s) {
               absl::StrAppend(out, PositionHeader(s), "\n", s.DebugString());
             });
  } else {
    puzzle::Solution answer = solver.Solve();
    if (answer.IsValid()) {
      LOG(INFO) << PositionHeader(answer);
    }
    LOG(INFO) << answer.DebugString();
    exit_code = answer.IsValid() ? 0 : 1;
  }

  LOG(INFO) << solver.DebugStatistics();
  
  return exit_code;
}
