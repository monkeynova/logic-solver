#include "puzzle/solver.h"

#include "absl/flags/flag.h"
#include "puzzle/base/all_match.h"
#include "puzzle/solution_permuter/solution_permuter_factory.h"

ABSL_FLAG(bool, puzzle_alternate_full_selectivity_check, true, "...");
ABSL_FLAG(bool, puzzle_alternate_disable, false, "...");

namespace puzzle {

Solver::Solver(EntryDescriptor entry_descriptor)
    : entry_descriptor_(std::move(entry_descriptor)),
      profiler_(Profiler::Create()) {
  alternates_.push_back(
      CreateSolutionPermuter(&entry_descriptor_, profiler_.get()));
  residual_.push_back({});
}

absl::Status Solver::AddFilter(SolutionFilter solution_filter) {
  filter_added_ = true;
  for (int i = 0; i < alternates_.size(); ++i) {
    if (current_alternate_ && i != current_alternate_->id_) {
      continue;
    }
    ASSIGN_OR_RETURN(bool fully_used,
                     alternates_[i]->AddFilter(solution_filter));
    if (!fully_used) {
      // Permuter indicated that it can't fully evaluate the filter.
      residual_[i].push_back(solution_filter);
    }
  }
  return absl::OkStatus();
}

absl::StatusOr<Solution> Solver::Solve() {
  ASSIGN_OR_RETURN(std::vector<Solution> ret, AllSolutions(1));
  if (ret.empty()) return absl::NotFoundError("No solution found");
  return std::move(ret[0]);
}

absl::StatusOr<Solver::AlternateId> Solver::PrepareAndChooseAlternate() {
  AlternateId ret;
  if (absl::GetFlag(FLAGS_puzzle_alternate_disable)) {
    ret.id_ = 0;
    RETURN_IF_ERROR(alternates_[0]->PrepareCheap());
    RETURN_IF_ERROR(alternates_[0]->PrepareFull());
    return ret;
  }

  double best_selectivity = 1.1;
  for (int i = 0; i < alternates_.size(); ++i) {
    RETURN_IF_ERROR(alternates_[i]->PrepareCheap());
    if (absl::GetFlag(FLAGS_puzzle_alternate_full_selectivity_check)) {
      RETURN_IF_ERROR(alternates_[i]->PrepareFull());
    }
    double alternate_selectivity = alternates_[i]->Selectivity();
    if (alternate_selectivity < best_selectivity) {
      ret.id_ = i;
      best_selectivity = alternate_selectivity;
    }
  }

  if (!absl::GetFlag(FLAGS_puzzle_alternate_full_selectivity_check)) {
    RETURN_IF_ERROR(alternates_[ret.id_]->PrepareFull());
  }

  VLOG_IF(1, alternates_.size() > 1) << "Chose alternate: " << ret.id_;

  return ret;
}

absl::StatusOr<std::vector<Solution>> Solver::AllSolutions(int limit) {
  ASSIGN_OR_RETURN(chosen_alternate_, PrepareAndChooseAlternate());

  SolutionPermuter* solution_permuter = alternates_[chosen_alternate_.id_].get();
  absl::Span<const SolutionFilter> on_solution = residual_[chosen_alternate_.id_];

  std::vector<Solution> ret;
  for (auto& solution : *solution_permuter) {
    VLOG(1) << "Solution found @" << solution.position();
    if (profiler_ != nullptr) {
      profiler_->NotePermutation(solution.position());
    }
    ++test_calls_;
    if (AllMatch(on_solution, solution)) {
      puzzle::Solution copy = solution.Clone();
      if (chosen_alternate_.id_ != 0) {
        ASSIGN_OR_RETURN(copy,
                         TransformAlternate(copy.Clone(), chosen_alternate_));
      }
      ret.push_back(std::move(copy));
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

  VLOG(1) << last_debug_statistics_;

  return ret;
}

std::string Solver::DebugStatistics() const { return last_debug_statistics_; }

Solver::AlternateId Solver::DefaultAlternate() const {
  AlternateId ret;
  ret.id_ = 0;
  return ret;
}

absl::StatusOr<Solver::AlternateId> Solver::CreateAlternate() {
  if (filter_added_) {
    return absl::FailedPreconditionError(
        "Cannot create alternative after adding a filter");
  }
  AlternateId ret;
  ret.id_ = alternates_.size();
  alternates_.push_back(
      CreateSolutionPermuter(&entry_descriptor_, profiler_.get()));
  residual_.push_back({});
  return ret;
}

absl::StatusOr<Solution> Solver::TransformAlternate(
    Solution in, AlternateId alternate) const {
  return absl::UnimplementedError("TransformAlternate not implemented");
}

}  // namespace puzzle
