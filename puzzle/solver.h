#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <memory>
#include <vector>

#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

class Solver {
 public:
  Solver();
  ~Solver() {}

  Solution Solve();
  std::vector<Solution> AllSolutions(int limit = -1);

  void SetIdentifiers(const Descriptor* id_descriptor) {
    entry_descriptor_.SetIds(id_descriptor);
  }

  void AddClass(int class_int, std::string class_name,
                const Descriptor* name_descriptor) {
    entry_descriptor_.SetClass(class_int, std::move(class_name),
                               name_descriptor);
  }

  // TODO(keith@monkeynova.com): Entry::Predicate rules have the structure
  // required to crop results in a single ClassPermuter if implemented a
  // skip function. That could be a significant speed up.
  // TODO(keith@monkeynova.com): Check class_int_restrict_list with a dummy
  // Solution that looks for class requests on other values.

  void AddAllEntryPredicate(std::string name, Entry::Predicate predicate,
                            std::vector<int> class_int_restrict_list = {}) {
    for (const int entry_id : entry_descriptor_.AllIds()->Values()) {
      AddSpecificEntryPredicate(name, entry_id, predicate,
                                class_int_restrict_list);
    }
  }

  void AddSpecificEntryPredicate(
      std::string name, int entry_id, Entry::Predicate predicate,
      std::vector<int> class_int_restrict_list = {}) {
    AddFilter(SolutionFilter(std::move(name), entry_id, predicate,
                             std::move(class_int_restrict_list)));
  }

  void AddPredicate(std::string name, Solution::Predicate predicate,
                    std::vector<int> class_int_restrict_list = {}) {
    AddFilter(SolutionFilter(std::move(name), predicate,
                             std::move(class_int_restrict_list)));
  }

  int test_calls() const { return test_calls_; }

  std::string DebugStatistics() const;

  const EntryDescriptor* entry_descriptor() const { return &entry_descriptor_; }

  // Takes ownership of 'descriptor' and ensures that it outlives 'solver'.
  template <class SpecificDescriptor>
  SpecificDescriptor* AddDescriptor(SpecificDescriptor* descriptor) {
    descriptors_.emplace_back(descriptor);
    return descriptor;
  }

 private:
  void AddFilter(SolutionFilter solution_filter);

  bool TestSolution(const Solution& s);
  EntryDescriptor entry_descriptor_;

  int test_calls_ = 0;

  std::string last_debug_statistics_;

  std::vector<SolutionFilter> on_solution_;

  std::unique_ptr<Profiler> profiler_;
  std::unique_ptr<SolutionPermuter> solution_permuter_;

  std::vector<std::unique_ptr<puzzle::Descriptor>> descriptors_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLVER_H
