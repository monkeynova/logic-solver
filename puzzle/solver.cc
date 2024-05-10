#include "puzzle/solver.h"

#include "puzzle/all_match.h"
#include "puzzle/solution_permuter_factory.h"

namespace puzzle {

Solver::Solver(EntryDescriptor entry_descriptor)
    : entry_descriptor_(std::move(entry_descriptor)),
      profiler_(Profiler::Create()),
      solution_permuter_(
          CreateSolutionPermuter(&entry_descriptor_, profiler_.get())) {}

absl::Status Solver::AddFilter(SolutionFilter solution_filter) {
  ASSIGN_OR_RETURN(bool full_used,
                   solution_permuter_->AddFilter(solution_filter));
  if (!full_used) {
    // Permuter indicated that it can't fully evaluate the filter.
    on_solution_.push_back(solution_filter);
  }
  return absl::OkStatus();
}

absl::StatusOr<Solution> Solver::Solve() {
  ASSIGN_OR_RETURN(std::vector<Solution> ret, AllSolutions(1));
  if (ret.empty()) return absl::NotFoundError("No solution found");
  return std::move(ret[0]);
}

absl::StatusOr<std::vector<Solution>> Solver::AllSolutions(int limit) {
  RETURN_IF_ERROR(solution_permuter_->Prepare());

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
