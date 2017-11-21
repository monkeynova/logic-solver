#include "puzzle/solver.h"

#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

#ifdef PROFILING
#include <sys/time.h>
#endif

namespace Puzzle {

Entry Entry::invalid_(-1);

bool Solver::TestSolution(const Solution& s) {
  return std::all_of(on_solution_.begin(),
		     on_solution_.end(),
		     [&s](const Solution::Predicate& p) {
		       return p(s);
		     });
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
  //BruteSolutionPermuter permuter(&entry_descriptor_);
  CroppedSolutionPermuter permuter(&entry_descriptor_, on_solution_with_class_);
#ifdef PROFILING
  struct timeval start;
  gettimeofday(&start, nullptr);
#endif
  auto it = std::find_if(permuter.begin(),
			 permuter.end(),
			 [this](const Solution& s) {
			   return this->TestSolution(s);
			 });
#ifdef PROFILING
  DumpProfiling(start);
#endif
  std::cout << "\033[1K\r" << std::flush;
  if (it != permuter.end()) {
    return *it;
  }
  return Solution();
}

std::vector<Solution> Solver::AllSolutions() {
  //BruteSolutionPermuter permuter(&entry_descriptor_);
  CroppedSolutionPermuter permuter(&entry_descriptor_, on_solution_with_class_);
#ifdef PROFILING
  struct timeval start;
  gettimeofday(&start, nullptr);
#endif
  std::vector<Solution> ret;
  
  std::copy_if(permuter.begin(),
	       permuter.end(),
	       back_inserter(ret),
	       [this](const Solution& s) {
		 return this->TestSolution(s);
	       });
#ifdef PROFILING
  DumpProfiling(start);
#endif
  std::cout << "\033[1K\r" << std::flush;
  return ret;
}

}  // namespace Puzzle
