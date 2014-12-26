#include "solver.h"

using namespace Puzzle;

EntryDescriptor EntryDescriptor::invalid_;

Entry Entry::invalid_(-1);

static vector<Entry> empty_entry_vector;
Solution Solution::invalid_(empty_entry_vector);

Solution Solver::Solve() {
    vector<Solution> solutions = possibleSolutions();
    auto it = find_if(solutions.begin(),
                      solutions.end(),
                      [this](const Solution& s) {
                          cout << "Trying" << endl << s.ToStr() << endl;
                          return all_of(onSolution.begin(),
                                        onSolution.end(),
                                        [s](const function<bool(const Solution&)>& p) { return p(s); } );
                      });
    if (it != solutions.end()) {
        return *it;
    }
    return Solution::Invalid();
}

vector<Solution> Solver::possibleSolutions() {
    return vector<Solution>();
}
