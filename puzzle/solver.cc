#include "puzzle/solver.h"

#include "puzzle/solution_permuter_factory.h"

namespace puzzle {

Solver::Solver()
  : profiler_(Profiler::Create()),
    solution_permuter_(CreateSolutionPermuter(&entry_descriptor_,
                                              profiler_.get())) {}

void Solver::AddPredicate(std::string name, Solution::Predicate predicate,
                          const std::vector<int>& class_int_restrict_list) {
  if (solution_permuter_->AddPredicate(name, predicate,
                                       class_int_restrict_list)) {
    // Permuter guarantees no need to evaluate the predicate further.
  } else {
    on_solution_.push_back(predicate);
  }
}

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
  solution_permuter_->Prepare();

  std::vector<Solution> ret;
  for (auto it = solution_permuter_->begin(); it != solution_permuter_->end(); ++it) {
    if (profiler_ != nullptr) {
      profiler_->NotePosition(it->permutation_position(),
                              it->permutation_count());
    }
    if (TestSolution(*it)) {
      ret.emplace_back(it->Clone());
      if (limit >=0 && ret.size() >= static_cast<size_t>(limit)) {
        break;
      }
    }
  }

  if (profiler_) {
    profiler_->NoteFinish();
  }

  last_debug_statistics_ =
      absl::StrCat("[", test_calls_, " solutions tested in ",
                   (profiler_
                    ? profiler_->Seconds()
                    : std::numeric_limits<double>::quiet_NaN()),
                   "s]");

  return ret;
}

std::string Solver::DebugStatistics() const {
  return last_debug_statistics_;
}

}  // namespace puzzle
