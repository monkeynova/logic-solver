#include "puzzle/solver.h"

#include <functional>

#include "gflags/gflags.h"
#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"
#include "puzzle/profiler.h"

DEFINE_bool(brute_force, false, "Brute force all possible solutions");

namespace Puzzle {

using std::placeholders::_1;

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
  if (FLAGS_brute_force) {
    return AllSolutionsImpl<BruteSolutionPermuter>(limit);
  } else {
    return AllSolutionsImpl<CroppedSolutionPermuter>(limit);
  }
}

template <class Permuter>
std::vector<Solution> Solver::AllSolutionsImpl(int limit) {
  Permuter permuter(&entry_descriptor_, on_solution_with_class_);
  auto profiler = Profiler::Create();

  std::vector<Solution> ret;
  for (auto it = permuter.begin(); it != permuter.end(); ++it) {
    profiler->NotePosition(it->permutation_position(),
                           it->permutation_count());
    if (TestSolution(*it)) {
      ret.emplace_back(it->Clone());
      if (limit != -1 && ret.size() >= limit) {
        break;
      }
    }
  }
  return ret;
}

std::string Solver::DebugStatistics() const {
  return absl::StrCat("[", test_calls_, " solutions tested]");
}

}  // namespace Puzzle
