#include "puzzle/solver.h"

#include <functional>

#include "gflags/gflags.h"
#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

DEFINE_bool(brute_force, false, "Brute force all possible solutions");

#ifdef PROFILING
#include <sys/time.h>
#endif

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

#ifdef PROFILING
static void DumpProfiling(const strict timeval& start) {
  struct timeval end;
  gettimeofday(&end, nullptr);
  double qps = s.permutation_position() /
    (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
  std::cout << "\033[1K\rTrying " << (100 * s.completion()) << "%, "
	    << qps/1000 << "Kqps" << std::flush;
}
#endif

Solution Solver::Solve() {
  if (FLAGS_brute_force) {
    return SolveImpl<BruteSolutionPermuter>();
  } else {
    return SolveImpl<CroppedSolutionPermuter>();
  }
}

template <class Permuter>
Solution Solver::SolveImpl() {
  Permuter permuter(&entry_descriptor_, on_solution_with_class_);
#ifdef PROFILING
  struct timeval start;
  gettimeofday(&start, nullptr);
#endif
  Solution ret;
  for (auto it = permuter.begin(); it != permuter.end(); ++it) {
    if (TestSolution(*it)) {
      ret = it->Clone();
      break;
    }
  }

#ifdef PROFILING
  DumpProfiling(start);
#endif
  std::cout << "\033[1K\r" << std::flush;

  return ret;
}

std::vector<Solution> Solver::AllSolutions() {
  if (FLAGS_brute_force) {
    return AllSolutionsImpl<BruteSolutionPermuter>();
  } else {
    return AllSolutionsImpl<CroppedSolutionPermuter>();
  }
}

template <class Permuter>
std::vector<Solution> Solver::AllSolutionsImpl() {
  Permuter permuter(&entry_descriptor_, on_solution_with_class_);
#ifdef PROFILING
  struct timeval start;
  gettimeofday(&start, nullptr);
#endif
  std::vector<Solution> ret;
  for (auto it = permuter.begin(); it != permuter.end(); ++it) {
    if (TestSolution(*it)) {
      ret.emplace_back(it->Clone());
    }
  }
#ifdef PROFILING
  DumpProfiling(start);
#endif
  std::cout << "\033[1K\r" << std::flush;
  return ret;
}

std::string Solver::DebugStatistics() const {
  return absl::StrCat("[", test_calls_, " solutions tested]");
}

}  // namespace Puzzle
