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
  ~Solver() = default;

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

  // TODO(@monkeynova): Check class_int_restrict_list with a dummy
  // Solution that looks for class requests on other values.

  // Add predicate with the constraint that only class values found in
  // `class_int_restrict_list` are used by this filter.
  void AddAllEntryPredicate(std::string name, Entry::Predicate predicate,
                            std::vector<int> class_int_restrict_list = {}) {
    for (const int entry_id : entry_descriptor_.AllIds()->Values()) {
      AddSpecificEntryPredicate(name, predicate, class_int_restrict_list,
                                entry_id);
    }
  }

  void AddSpecificEntryPredicate(std::string name, Entry::Predicate predicate,
                                 std::vector<int> class_int_restrict_list,
                                 int entry_id) {
    AddFilter(SolutionFilter(std::move(name), predicate,
                             std::move(class_int_restrict_list), entry_id));
  }

  // Add predicate with the constraint that only class values found in
  // `class_int_restrict_list` are used by this filter.
  void AddPredicate(std::string name, Solution::Predicate predicate,
                    std::vector<int> class_int_restrict_list = {}) {
    AddFilter(SolutionFilter(std::move(name), predicate,
                             std::move(class_int_restrict_list)));
  }
  void AddPredicate(std::string name, Solution::Predicate predicate,
                    std::initializer_list<int> class_int_restrict_list) {
    AddPredicate(std::move(name), predicate,
                 std::vector<int>(class_int_restrict_list));
  }

  // Add predicate with the constraint that only class values found in the
  // keys of `class_to_entry` are used by this filter and that if a false
  // value is found the correponding value represents and entry which may
  // be skipped.
  void AddPredicate(std::string name, Solution::Predicate predicate,
                    absl::flat_hash_map<int, int> class_to_entry) {
    AddFilter(
        SolutionFilter(std::move(name), predicate, std::move(class_to_entry)));
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
