#include "puzzle/solver.h"

#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

#include <sys/time.h>

using namespace Puzzle;

Entry Entry::invalid_(-1);

bool Solver::TestSolution(const Solution& s, const struct timeval &start) {
  bool is_solution = std::all_of(on_solution_.begin(),
				 on_solution_.end(),
				 [&s](const Predicate& p) { return p(s); } );
#ifdef PROFILE
  if (attempts > 1e8) {
    is_solution = 1;
  }
#endif
  if (is_solution || s.permutation_position() % 777777 == 0) {
    struct timeval end;
    gettimeofday(&end,nullptr);
    double qps = s.permutation_position() / (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
    std::cout << "\033[1K\rTrying " << (100 * s.completion()) << "%, " << qps/1000 << "Kqps" << std::flush;
  }
  
  return is_solution;
}

Solution Solver::Solve() {
  //BruteSolutionPermuter permuter(&entry_descriptor_);
  CroppedSolutionPermuter permuter(&entry_descriptor_, on_solution_with_class_);
  struct timeval start;
  gettimeofday(&start,nullptr);
  auto it = std::find_if(permuter.begin(),
			 permuter.end(),
			 [this,start](const Solution& s) { return this->TestSolution(s, start); });
  std::cout << "\033[1K\r" << std::flush;
  if (it != permuter.end()) {
    return *it;
  }
  return Solution();
}

std::vector<Solution> Solver::AllSolutions() {
  //BruteSolutionPermuter permuter(&entry_descriptor_);
  CroppedSolutionPermuter permuter(&entry_descriptor_, on_solution_with_class_);
  struct timeval start;
  gettimeofday(&start,nullptr);
  std::vector<Solution> ret;
  
  std::copy_if(permuter.begin(),
	       permuter.end(),
	       back_inserter(ret),
	       [this,start](const Solution& s) { return this->TestSolution(s, start); });
  std::cout << "\033[1K\r" << std::flush;
  return ret;
}
