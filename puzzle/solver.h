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
        on_solution_with_class_.resize(class_int);
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
    void AddPredicate(function<bool(const Entry&)> predicate, const vector<int>& class_int_restrict_list) {
        AddPredicate([predicate](const Solution& s) {
                return all_of(s.entries().begin(),
                              s.entries().end(),
                              predicate);
            }, class_int_restrict_list);
    }
    void AddPredicate(Predicate predicate) {
        on_solution_.push_back(predicate);
    }
    void AddPredicate(Predicate predicate, int class_int_restrict) {
        vector<int> class_int_restrict_list = {class_int_restrict};
        AddPredicate(predicate, class_int_restrict_list);
    }
    void AddPredicate(Predicate predicate, const vector<int>& class_int_restrict_list) {
        on_solution_.push_back(predicate);
        on_solution_with_class_.push_back(std::pair<Predicate,vector<int>>(predicate, class_int_restrict_list));
    }

    Solution Solve();

    private:
    EntryDescriptor entry_descriptor_;

    vector<pair<Predicate,vector<int>>> on_solution_with_class_;
    vector<Predicate> on_solution_;
};

}  // namespace Puzzle

#endif  // __PUZZLE_SOLVER_H
