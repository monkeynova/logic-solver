#include "puzzle/solver.h"

#include "puzzle/all_match.h"
#include "puzzle/solution_permuter_factory.h"

namespace puzzle {

Solver::Solver()
    : profiler_(Profiler::Create()),
      solution_permuter_(
          CreateSolutionPermuter(&entry_descriptor_, profiler_.get())) {}

void Solver::AddFilter(SolutionFilter solution_filter) {
  if (solution_permuter_->AddFilter(solution_filter)) {
    // Permuter guarantees no need to evaluate the predicate further.
  } else {
    on_solution_.push_back(solution_filter);
  }
}

Solution Solver::Solve() {
  std::vector<Solution> ret = AllSolutions(1);
  if (ret.empty()) {
    return Solution();
  }
  return std::move(ret[0]);
}

std::vector<Solution> Solver::AllSolutions(int limit) {
  solution_permuter_->Prepare();

  std::vector<Solution> ret;
  for (auto it = solution_permuter_->begin(); it != solution_permuter_->end();
       ++it) {
    if (profiler_ != nullptr) {
      profiler_->NotePosition(it->permutation_position(),
                              it->permutation_count());
    }
    ++test_calls_;
    if (AllMatch(on_solution_, *it)) {
      ret.emplace_back(it->Clone());
      if (limit >= 0 && ret.size() >= static_cast<size_t>(limit)) {
        break;
      }
    }
  }

  if (profiler_) {
    profiler_->NoteFinish();
  }

  last_debug_statistics_ =
      absl::StrCat("[", test_calls_, " solutions tested in ",
                   (profiler_ ? profiler_->Seconds()
                              : std::numeric_limits<double>::quiet_NaN()),
                   "s]");

  return ret;
}

std::string Solver::DebugStatistics() const { return last_debug_statistics_; }

}  // namespace puzzle
