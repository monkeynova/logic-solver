#ifndef PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H
#define PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H

#include "puzzle/base/solution.h"
#include "puzzle/solution_permuter/solution_permuter.h"

namespace puzzle {

class AllowedValueSolutionPermuter final : public SolutionPermuter {
 public:
  explicit AllowedValueSolutionPermuter(const EntryDescriptor* e);
  ~AllowedValueSolutionPermuter() = default;

  // Movable, but not copyable.
  AllowedValueSolutionPermuter(const AllowedValueSolutionPermuter&) = delete;
  AllowedValueSolutionPermuter& operator=(const AllowedValueSolutionPermuter&) = delete;
  AllowedValueSolutionPermuter(AllowedValueSolutionPermuter&&) = default;
  AllowedValueSolutionPermuter& operator=(AllowedValueSolutionPermuter&&) = default;

  absl::Status PrepareCheap() override;
  absl::Status PrepareFull() override;

  absl::StatusOr<bool> AddFilter(SolutionFilter solution_filter) override;

  double Selectivity() const override { return 1.0; }

  iterator begin() const override;

  double permutation_count() const;

 private:
  friend class AllowedValueAdvancer;
  // {id: {class: allowed_value_bv}}
  std::vector<std::vector<int>> allowed_bv_;
};

}  // namespace puzzle

#endif   // PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H