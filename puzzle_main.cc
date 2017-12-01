#include <iostream>
#include <memory>
#include <vector>

#include "gflags/gflags.h"
#include "puzzle/solver.h"

DEFINE_bool(all, false, "Show all solutions");

extern void SetupProblem(Puzzle::Solver* s,
                         std::vector<std::unique_ptr<Puzzle::Descriptor>> *descriptors);
extern void AddProblemPredicates(Puzzle::Solver* s);
extern void AddRulePredicates(Puzzle::Solver* s);

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  Puzzle::Solver solver;
  std::vector<std::unique_ptr<Puzzle::Descriptor>> descriptors;
  
  SetupProblem(&solver, &descriptors);
  AddProblemPredicates(&solver);
  AddRulePredicates(&solver);
  
  int exit_code = 1;
  
  if (FLAGS_all) {
    std::cout << "[AllSolutions]" << std::endl;
    std::vector<Puzzle::Solution> all_solutions = solver.AllSolutions();
    exit_code = all_solutions.size() > 0 ? 0 : 1;
    std::cout << "[" << all_solutions.size() << " solutions]"
	      << std::endl;
    std::cout
      << absl::StrJoin(
	     all_solutions, "\n",
	     [](std::string* out, const Puzzle::Solution& s) {
	       absl::StrAppend(out, s.ToStr());
	     })
      << std::endl;
  } else {
    Puzzle::Solution answer = solver.Solve();
    std::cout << answer.ToStr() << std::endl;
    exit_code = answer.IsValid() ? 0 : 1;
  }

  std::cout << "[tested " << solver.test_calls() << " solutions]" << std::endl;
  
  return exit_code;
}
