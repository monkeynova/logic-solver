#ifndef __PUZZLE_SOLVER_H
#define __PUZZLE_SOLVER_H

#include "puzzle/solution.h"

namespace Puzzle {

class Solver {
    public:
    Solver() {}
    ~Solver() {}

    void SetIdentifiers(const Descriptor* id_descriptor) {
        entry_descriptor_.SetIds(id_descriptor);
    }
    void AddClass(int class_int, const string& class_name, const Descriptor* name_descriptor) {
        entry_descriptor_.SetClass(class_int, class_name, name_descriptor);
    }
    void AddPredicate(function<bool(const Entry&)> predicate) {
        AddPredicate([predicate](const Solution& s) {
                return all_of(s.entries().begin(),
                              s.entries().end(),
                              predicate);
            });
    }
    void AddPredicate(function<bool(const Entry&)> predicate, int class_int_restrict) {
        AddPredicate([predicate](const Solution& s) {
                return all_of(s.entries().begin(),
                              s.entries().end(),
                              predicate);
            }, class_int_restrict);
    }
    void AddPredicate(function<bool(const Entry&)> predicate, const vector<int>& class_int_restricts) {
        AddPredicate([predicate](const Solution& s) {
                return all_of(s.entries().begin(),
                              s.entries().end(),
                              predicate);
            }, class_int_restricts);
    }
    void AddPredicate(function<bool(const Solution&)> predicate) {
        onSolution.push_back(predicate);
    }
    void AddPredicate(function<bool(const Solution&)> predicate, int class_int_restrict) {
        onSolution.push_back(predicate);
    }
    void AddPredicate(function<bool(const Solution&)> predicate, const vector<int>& class_int_restricts) {
        onSolution.push_back(predicate);
    }

    Solution Solve();

    private:
    EntryDescriptor entry_descriptor_;

    vector<function<bool(const Solution&)>> onSolution;
};

}  // namespace Puzzle

#endif  // __PUZZLE_SOLVER_H
