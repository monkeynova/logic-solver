#include "solver.h"

using namespace Puzzle;

Entry Entry::invalid_(-1);

static vector<Entry> empty_entry_vector;
Solution Solution::invalid_(empty_entry_vector);

ClassPermuter::iterator::iterator(const Descriptor* descriptor)
    : descriptor_(descriptor) {

}

void ClassPermuter::iterator::Advance() {

}

SolutionPermuter::iterator::iterator(const EntryDescriptor* entry_descriptor)
    : entry_descriptor_(entry_descriptor), current_(Solution::Invalid()) {
    if (entry_descriptor_ == nullptr) {
        return;
    }

    vector<int> empty;
    
    for (auto id: entry_descriptor_->AllIds().Values()) {
        entries_.push_back(Entry(id,empty,entry_descriptor_));
    }
    for (auto class_int: entry_descriptor_->AllClasses().Values()) {
        const Descriptor& class_descriptor = entry_descriptor_->AllClassValues(class_int);
        permuters_.push_back(ClassPermuter(&class_descriptor));
        iterators_.push_back(permuters_[permuters_.size() - 1].begin());
    }

    current_ = Solution(entries_);
}

void SolutionPermuter::iterator::Advance() {
    bool at_end = true;
    for (int i = 0; i < permuters_.size(); ++i) {
        ++iterators_[i];
        
        bool carry = false;
        if (iterators_[i] == permuters_[i].end()) {
            iterators_[i] = permuters_[i].begin();
            carry = true;
        }

        for (int j = 0; j < iterators_[i]->size(); j++ ) {
            entries_[j].SetClass(i, (*iterators_[i])[j]);
        }

        if (!carry) {
            at_end = false;
            break;
        }
    }
    if (at_end) {
        current_ = Solution::Invalid();
    }
}

Solution Solver::Solve() {
    SolutionPermuter permuter(&entry_descriptor_);
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
