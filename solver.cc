#include "solver.h"

using namespace Puzzle;

EntryDescriptor EntryDescriptor::invalid_;

Entry Entry::invalid_(-1);

static vector<Entry> empty_entry_vector;
Solution Solution::invalid_(empty_entry_vector);

SolutionPermuter::Iterator::Iterator(const EntryDescriptor& entry_descriptor)
    : entry_descriptor_(entry_descriptor), current_(Solution::Invalid()) {
    vector<int> empty;

    for (auto id: entry_descriptor_.AllIds().Values()) {
        entries_.push_back(Entry(id,empty,entry_descriptor_));
    }
}

void SolutionPermuter::Iterator::Advance() {

}

Solution Solver::Solve() {
    SolutionPermuter permuter(entry_descriptor_);
    auto it = find_if(permuter.begin(),
                      permuter.end(),
                      [this](const Solution& s) {
                          cout << "Trying" << endl << s.ToStr() << endl;
                          return all_of(onSolution.begin(),
                                        onSolution.end(),
                                        [s](const function<bool(const Solution&)>& p) { return p(s); } );
                      });
    if (it != permuter.end()) {
        return *it;
    }
    return Solution::Invalid();
}
