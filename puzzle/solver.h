#ifndef __PUZZLE_SOLVER_H
#define __PUZZLE_SOLVER_H

#include "puzzle/solution.h"

namespace Puzzle {

class Solver {
 public:
  Solver() {}
  ~Solver() {}

  Solution Solve();
  std::vector<Solution> AllSolutions(int limit = -1);

  void SetIdentifiers(const Descriptor* id_descriptor) {
    entry_descriptor_.SetIds(id_descriptor);
  }

  void AddClass(int class_int, const std::string& class_name,
                const Descriptor* name_descriptor) {
    entry_descriptor_.SetClass(class_int, class_name, name_descriptor);
  }

  void AddPredicate(std::string name, Entry::Predicate predicate) {
    AddPredicate(name, [predicate](const Solution& s) {
        return all_of(s.entries().begin(),
                      s.entries().end(),
                      predicate);
      });
  }

  void AddPredicate(std::string name, Entry::Predicate predicate,
                    int class_int_restrict) {
    AddPredicate(name, [predicate](const Solution& s) {
        return all_of(s.entries().begin(),
                      s.entries().end(),
                      predicate);
      }, class_int_restrict);
  }

  void AddPredicate(std::string name, Entry::Predicate predicate,
                    const std::vector<int>& class_int_restrict_list) {
    AddPredicate(name, [predicate](const Solution& s) {
        return all_of(s.entries().begin(),
                      s.entries().end(),
                      predicate);
      }, class_int_restrict_list);
  }

  void AddPredicate(std::string name, Solution::Predicate predicate) {
    on_solution_.push_back(predicate);
  }

  void AddPredicate(std::string name, Solution::Predicate predicate,
                    int class_int_restrict) {
    std::vector<int> class_int_restrict_list = {class_int_restrict};
    AddPredicate(name, predicate, class_int_restrict_list);
  }

  void AddPredicate(std::string name, Solution::Predicate predicate,
                    const std::vector<int>& class_int_restrict_list) {
    on_solution_.push_back(predicate);
    on_solution_with_class_.emplace_back(name, predicate,
                                         class_int_restrict_list);
  }

  int test_calls() const { return test_calls_; }

  std::string DebugStatistics() const;
  
 private:
  template <class Permuter>
  std::vector<Solution> AllSolutionsImpl(int limit);

  bool TestSolution(const Solution& s);
  EntryDescriptor entry_descriptor_;

  int test_calls_;
  
  std::vector<Solution::Cropper> on_solution_with_class_;
  std::vector<Solution::Predicate> on_solution_;
};

}  // namespace Puzzle

#endif  // __PUZZLE_SOLVER_H
