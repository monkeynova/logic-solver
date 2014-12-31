#include "puzzle/solver.h"

#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

#include <sys/time.h>

using namespace Puzzle;

Entry Entry::invalid_(-1);

Solution Solver::Solve() {
    //BruteSolutionPermuter permuter(&entry_descriptor_);
    CroppedSolutionPermuter permuter(&entry_descriptor_, on_solution_with_class_);
    struct timeval start;
    gettimeofday(&start,nullptr);
    auto it = find_if(permuter.begin(),
                      permuter.end(),
                      [this,start](const Solution& s) {
                          bool is_solution = all_of(on_solution_.begin(),
                                                    on_solution_.end(),
                                                    [&s](const function<bool(const Solution&)>& p) { return p(s); } );
#ifdef PROFILE
                          if (attempts > 1e8) {
                              is_solution = 1;
                          }
#endif
                          if (is_solution || s.permutation_position() % 777777 == 0) {
                              struct timeval end;
                              gettimeofday(&end,nullptr);
                              double qps = s.permutation_position() / (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
                              cout << "\033[1K\rTrying " << (100 * s.completion()) << "%, " << qps/1000 << "Kqps" << flush;
                          }

                          return is_solution;
                      });
    cout << endl;
    if (it != permuter.end()) {
        return *it;
    }
    return Solution();
}
