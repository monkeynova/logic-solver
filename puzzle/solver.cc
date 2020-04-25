#include "puzzle/solver.h"

#include "puzzle/solution_permuter_factory.h"

namespace puzzle {

Entry Entry::invalid_(-1);

bool Solver::TestSolution(const Solution& s) {
  ++test_calls_;
  bool ret = std::all_of(on_solution_.begin(),
                     on_solution_.end(),
                     [&s](const Solution::Predicate& p) {
                       return p(s);
                     });
  return ret;
}

Solution Solver::Solve() {
  std::vector<Solution> ret = AllSolutions(1);
  if (ret.empty()) {
    return Solution();
  }
  return std::move(ret[0]);
}

std::vector<Solution> Solver::AllSolutions(int limit) {
  std::unique_ptr<Profiler> profiler = Profiler::Create();

  std::unique_ptr<SolutionPermuter> permuter = CreateSolutionPermuter(
	&entry_descriptor_, on_solution_with_class_, profiler.get());

  std::vector<Solution> ret;
  for (auto it = permuter->begin(); it != permuter->end(); ++it) {
    if (profiler != nullptr) {
      profiler->NotePosition(it->permutation_position(),
                             it->permutation_count());
    }
    if (TestSolution(*it)) {
      ret.emplace_back(it->Clone());
      if (limit >=0 && ret.size() >= static_cast<size_t>(limit)) {
        break;
      }
    }
  }

  if (profiler) {
    profiler->NoteFinish();
  }

  last_debug_statistics_ =
      absl::StrCat("[", test_calls_, " solutions tested in ",
                   profiler ? profiler->Seconds() : std::numeric_limits<double>::quiet_NaN(),
		   "s]");

  return ret;
}

std::string Solver::DebugStatistics() const {
  return last_debug_statistics_;
}

}  // namespace puzzle
