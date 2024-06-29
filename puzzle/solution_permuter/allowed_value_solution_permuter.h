#ifndef PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H
#define PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H

#include "puzzle/base/solution.h"
#include "puzzle/solution_permuter/solution_permuter.h"

namespace puzzle {

class AllowedValueGrid {
 public:
  struct Box {
    int entry_id;
    int class_id;
  };

  class Undo {
   public:
    int NextVal() const;
    Box box() const { return box_; }
    int entry_id() const { return box_.entry_id; }
    int class_id() const { return box_.class_id; }

   private:
    friend AllowedValueGrid;
    Box box_;
    int bv_;
    int val_;
  };

  AllowedValueGrid() = default;
  AllowedValueGrid(const EntryDescriptor* descriptor);

  void SetMutableSolution(MutableSolution* mutable_solution);

  Undo Empty(Box box);
  int FirstVal(Box box);
  std::pair<Undo, bool> Assign(Box box, int value);
  void UnAssign(Undo undo);

  void AddFilter(SolutionFilter solution_filter, std::vector<Box> input);

 private:
  // {id: {class: allowed_value_bv}}
  std::vector<std::vector<int>> bv_;
  std::vector<std::vector<bool>> assigned_;
  std::vector<std::vector<int>> vals_;
  std::vector<std::vector<std::vector<std::pair<SolutionFilter, std::vector<Box>>>>> solution_filters_;
  MutableSolution* mutable_solution_ = nullptr;
};

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
  AllowedValueGrid allowed_grid_;
};

class AllowedValueAdvancer final : public SolutionPermuter::AdvancerBase {
 public:
  explicit AllowedValueAdvancer(const AllowedValueSolutionPermuter* permuter,
                                const EntryDescriptor* entry_descriptor);
  AllowedValueAdvancer(const AllowedValueAdvancer&) = delete;

  AllowedValueAdvancer& operator=(const AllowedValueAdvancer&) = delete;
  Position position() const;

 private:
  void Advance() override;
  bool Undo2Reassign();
  bool Reassign2Undo();

  AllowedValueGrid allowed_grid_;
  std::vector<AllowedValueGrid::Undo> undos_;
  std::vector<AllowedValueGrid::Undo> reassign_;
};

}  // namespace puzzle

#endif   // PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H