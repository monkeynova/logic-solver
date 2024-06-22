#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "puzzle/base/profiler.h"
#include "puzzle/base/solution.h"
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

 protected:
  // The following methods allow for constructing multiple concurrent
  // concurrent representations of the filters uses to specify the problem
  // being solved. If utilized, multiple `SolutionPermuter`s are constructed
  // and `Prepare`d and the one with the smallest `Selectivity` is used
  // to iterate through results.
  // This is used to allow Sudoku and other Grid problems to make a dynamic
  // choice between a default representation of the problem and the transpose
  // which is equivalent, but might have different filter performance depending
  // on which axis is aligned with the `Entry`s.
  // Each alternate version of the filters corresponds to a single AlternateId.
  class AlternateId {
   public:
    bool operator==(const AlternateId&) const = default;

   private:
    friend class Solver;
    int id_;
  };

  // Returns the default AlternateId, that is the one created by constructing
  // this Solver, as opposed to any constructed by CreateAlternate.
  AlternateId DefaultAlternate() const;
  // Creates another version of the problem to solve. Must be called before any
  // filters have been added.
  absl::StatusOr<AlternateId> CreateAlternate();
  // Sets the current behavior when adding filters. All subsequent
  // Add*Predicate alls affect only `alternate`. If `alternate` is std::nullopt
  // resets to having Add*Predicate calls add to all alternates.
  void SetAlternate(std::optional<AlternateId> alternate) {
    current_alternate_ = alternate;
  }
  // Called on any solution found if the chosen alternate was not the default.
  // `in` is the result that was created to be returned `Solve`.
  // `alternate` represents which choice was used to create `in`.
  // This call must return the solution as it should be returned from `Solve`.
  // As an example, for Grid transposition, this un-transposes the result.
  virtual absl::StatusOr<Solution> TransformAlternate(
      Solution in, AlternateId alternate) const;

 private:
  absl::Status AddFilter(SolutionFilter solution_filter);

  const EntryDescriptor entry_descriptor_;

  bool filter_added_ = false;
  int test_calls_ = 0;

  std::string last_debug_statistics_;

  std::unique_ptr<Profiler> profiler_;

  std::optional<AlternateId> current_alternate_;
  std::vector<std::unique_ptr<SolutionPermuter>> alternates_;
  std::vector<std::vector<SolutionFilter>> residual_;
  AlternateId chosen_alternate_;

  std::vector<std::unique_ptr<puzzle::Descriptor>> descriptors_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLVER_H
