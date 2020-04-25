#ifndef __PUZZLE_SOLVER_H
#define __PUZZLE_SOLVER_H

#include <memory>
#include <vector>

#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

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

  // TODO(keith@monkeynova.com): Entry::Predicate rules have the structure
  // required to crop results in a single ClassPermuter if implemented a
  // skip function. That would be a significant speed up.
  // TODO(keith@monkeynova.com): Also add a model where a Entry::Predicate
  // can be combined with a specific id rather than having to hunt it down
  // in the Soluition.
  // TODO(keith@monkeynova.com): Check class_int_restrict_list with a dummy
  // Solution that looks for class requests on other values.

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

  const EntryDescriptor* entry_descriptor() const {
    return &entry_descriptor_;
  }

  // Takes ownership of 'descriptor' and ensures that it outlives 'solver'.
  template <class SpecificDescriptor>
  SpecificDescriptor* AddDescriptor(SpecificDescriptor* descriptor) {
    descriptors_.emplace_back(descriptor);
    return descriptor;
  }

 private:
  bool TestSolution(const Solution& s);
  EntryDescriptor entry_descriptor_;

  int test_calls_ = 0;

  std::string last_debug_statistics_;

  std::vector<Solution::Cropper> on_solution_with_class_;
  std::vector<Solution::Predicate> on_solution_;

  std::vector<std::unique_ptr<puzzle::Descriptor>> descriptors_;
};

}  // namespace puzzle

#endif  // __PUZZLE_SOLVER_H
