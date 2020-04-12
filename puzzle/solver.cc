#include "puzzle/solver.h"

#include <functional>
#include <limits>

#include "gflags/gflags.h"
#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

DEFINE_bool(puzzle_brute_force, false, "Brute force all possible solutions");

namespace puzzle {

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
  std::unique_ptr<Profiler> profiler = Profiler::Create();

  std::vector<Solution> ret;
  if (FLAGS_puzzle_brute_force) {
    BruteSolutionPermuter permuter(&entry_descriptor_);
    ret = AllSolutionsImpl(limit, profiler.get(), &permuter);
  } else {
    CroppedSolutionPermuter permuter(&entry_descriptor_,
                                     on_solution_with_class_,
                                     profiler.get());
    ret = AllSolutionsImpl(limit, profiler.get(), &permuter);
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

template <class Permuter>
std::vector<Solution> Solver::AllSolutionsImpl(int limit, Profiler* profiler,
                                               Permuter* permuter) {

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
  return ret;
}

std::string Solver::DebugStatistics() const {
  return last_debug_statistics_;
}

}  // namespace puzzle
