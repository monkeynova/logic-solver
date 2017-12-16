#include "puzzle/solver.h"

#include <functional>

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
  profiler_ = Profiler::Create();

  std::vector<Solution> ret;
  if (FLAGS_puzzle_brute_force) {
    BruteSolutionPermuter permuter(&entry_descriptor_);
    ret = AllSolutionsImpl(limit, &permuter);
  } else {
    CroppedSolutionPermuter permuter(&entry_descriptor_,
                                     on_solution_with_class_,
                                     profiler_.get());
    ret = AllSolutionsImpl(limit, &permuter);
  }

  profiler_->NoteFinish();
  
  return ret;
}

template <class Permuter>
std::vector<Solution> Solver::AllSolutionsImpl(int limit, Permuter* permuter) {

  std::vector<Solution> ret;
  for (auto it = permuter->begin(); it != permuter->end(); ++it) {
    profiler_->NotePosition(it->permutation_position(),
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
  return absl::StrCat("[", test_calls_, " solutions tested in ",
		      (profiler_ == nullptr ? -1 : profiler_->Seconds()), "s]");
}

}  // namespace puzzle
