#include "puzzle/solver.h"

#include "puzzle/brute_solution_permuter.h"

#include <sys/time.h>

using namespace Puzzle;

Entry Entry::invalid_(-1);

Solution Solver::Solve() {
    BruteSolutionPermuter permuter(&entry_descriptor_);
    long long total = permuter.permutation_count();
    long long attempts = 0;
    struct timeval start;
    gettimeofday(&start,nullptr);
    auto it = find_if(permuter.begin(),
                      permuter.end(),
                      [this,&attempts,total,start](const Solution& s) {
                          if (++attempts % 777777 == 0) {
                              struct timeval end;
                              gettimeofday(&end,nullptr);
                              double qps = attempts / (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
                              cout << "\033[1K\rTrying " << (100 * attempts / static_cast<double>(total)) << "%, " << qps/1000 << "Kqps" << flush;
                          }
#ifdef PROFILE
                          if (attempts > 1e8) {
                              return true;
                          }
#endif
                          return all_of(onSolution.begin(),
                                        onSolution.end(),
                                        [&s](const function<bool(const Solution&)>& p) { return p(s); } );
                      });
    cout << endl;
    if (it != permuter.end()) {
        return *it;
    }
    return Solution();
}
