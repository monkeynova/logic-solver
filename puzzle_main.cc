#include <iostream>
#include <memory>
#include <vector>

#include "gflags/gflags.h"
#include "puzzle/instance.h"

DEFINE_bool(all, false, "Show all solutions");

extern void SetupProblem(puzzle::Solver* s);

std::string PositionHeader(const puzzle::Solution& s) {
  return absl::StrCat("[position=", s.permutation_position(), "/",
                      s.permutation_count(), " (", s.Completion(), ")]");
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  puzzle::Solver solver;
  
  SetupProblem(&solver);
  
  int exit_code = 1;
  
  if (FLAGS_all) {
    std::cout << "[AllSolutions]" << std::endl;
    std::vector<puzzle::Solution> all_solutions = solver.AllSolutions();
    exit_code = all_solutions.size() > 0 ? 0 : 1;
    std::cout << "[" << all_solutions.size() << " solutions]"
	      << std::endl;
    std::cout
      << absl::StrJoin(
	     all_solutions, "\n",
	     [](std::string* out, const puzzle::Solution& s) {
	       absl::StrAppend(out, PositionHeader(s), "\n", s.ToStr());
	     })
      << std::endl;
  } else {
    puzzle::Solution answer = solver.Solve();
    if (answer.IsValid()) {
      std::cout << PositionHeader(answer) << std::endl;
    }
    std::cout << answer.ToStr() << std::endl;
    exit_code = answer.IsValid() ? 0 : 1;
  }

  std::cout << solver.DebugStatistics() << std::endl;
  
  return exit_code;
}
