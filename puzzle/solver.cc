#include "puzzle/solver.h"

#include "puzzle/all_match.h"
#include "puzzle/solution_permuter_factory.h"

namespace puzzle {

Solver::Solver()
    : profiler_(Profiler::Create()),
      solution_permuter_(
          CreateSolutionPermuter(&entry_descriptor_, profiler_.get())) {}

absl::Status Solver::AddFilter(SolutionFilter solution_filter) {
  absl::StatusOr<bool> added = solution_permuter_->AddFilter(solution_filter);
  if (!added.ok()) return added.status();
  if (*added) {
    // Permuter guarantees no need to evaluate the predicate further.
  } else {
    on_solution_.push_back(solution_filter);
  }
  return absl::OkStatus();
}

absl::StatusOr<Solution> Solver::Solve() {
  absl::StatusOr<std::vector<Solution>> ret = AllSolutions(1);
  if (!ret.ok()) return ret.status();
  if (ret->empty()) return absl::NotFoundError("No solution found");
  return std::move(ret->at(0));
}

absl::StatusOr<std::vector<Solution>> Solver::AllSolutions(int limit) {
  if (absl::Status st = solution_permuter_->Prepare(); !st.ok()) return st;

  std::vector<Solution> ret;
  for (auto it = solution_permuter_->begin(); it != solution_permuter_->end();
       ++it) {
    if (profiler_ != nullptr) {
      profiler_->NotePermutation(it->permutation_position(),
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
