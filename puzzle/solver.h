#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

class Solver {
 public:
  explicit Solver(EntryDescriptor entry_descriptor);
  ~Solver() = default;

  absl::StatusOr<Solution> Solve();
  absl::StatusOr<std::vector<Solution>> AllSolutions(int limit = -1);

  // TODO(@monkeynova): Check class_int_restrict_list with a dummy
  // Solution that looks for class requests on other values.

  // Add predicate with the constraint that only class values found in
  // `class_int_restrict_list` are used by this filter.
  absl::Status AddAllEntryPredicate(
      std::string name, Entry::Predicate predicate,
      std::vector<int> class_int_restrict_list = {}) {
    for (int entry_id = 0; entry_id < entry_descriptor_.AllIds()->size();
         ++entry_id) {
      absl::Status added = AddSpecificEntryPredicate(
          name, predicate, class_int_restrict_list, entry_id);
      if (!added.ok()) return added;
    }
    return absl::OkStatus();
  }

  absl::Status AddSpecificEntryPredicate(
      std::string name, Entry::Predicate predicate,
      std::vector<int> class_int_restrict_list, int entry_id) {
    return AddFilter(SolutionFilter(std::move(name), predicate,
                                    std::move(class_int_restrict_list),
                                    entry_id));
  }

  // Add predicate with the constraint that only class values found in
  // `class_int_restrict_list` are used by this filter.
  absl::Status AddPredicate(std::string name, Solution::Predicate predicate,
                            std::vector<int> class_int_restrict_list = {}) {
    return AddFilter(SolutionFilter(std::move(name), predicate,
                                    std::move(class_int_restrict_list)));
  }
  absl::Status AddPredicate(
      std::string name, Solution::Predicate predicate,
      std::initializer_list<int> class_int_restrict_list) {
    return AddPredicate(std::move(name), predicate,
                        std::vector<int>(class_int_restrict_list));
  }

  // Add predicate with the constraint that only class values found in the
  // keys of `class_to_entry` are used by this filter and that if a false
  // value is found the correponding value represents and entry which may
  // be skipped.
  absl::Status AddPredicate(std::string name, Solution::Predicate predicate,
                            absl::flat_hash_map<int, int> class_to_entry) {
    return AddFilter(
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
  absl::Status AddFilter(SolutionFilter solution_filter);

  const EntryDescriptor entry_descriptor_;

  int test_calls_ = 0;

  std::string last_debug_statistics_;

  std::vector<SolutionFilter> on_solution_;

  std::unique_ptr<Profiler> profiler_;
  std::unique_ptr<SolutionPermuter> solution_permuter_;

  std::vector<std::unique_ptr<puzzle::Descriptor>> descriptors_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLVER_H
