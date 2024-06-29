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

    template <typename Sink>
    friend void AbslStringify(Sink& sink, const Box& b) {
      absl::Format(&sink, "{%v,%v}", b.entry_id, b.class_id);
    }
  };

  class Undo {
   public:
    Undo() = default;
  
    Undo(Undo&) = delete;
    Undo& operator=(Undo&) = delete;
    Undo(Undo&&) = default;
    Undo& operator=(Undo&&) = default;

    int NextVal() const;
    Box box() const { return box_; }
    int entry_id() const { return box_.entry_id; }
    int class_id() const { return box_.class_id; }

    template <typename Sink>
    friend void AbslStringify(Sink& sink, const Undo& u) {
      absl::Format(&sink, "{%v,%d/0x%x}", u.box_, u.val_, u.bv_);
    }

   private:
    friend AllowedValueGrid;
    Box box_;
    int bv_;
    int val_;
    std::vector<std::pair<Box, int>> restore;
  };

  explicit AllowedValueGrid(MutableSolution* mutable_solution);

  Position position() const;

  Undo Empty(Box box) const;
  int FirstVal(Box box) const;

  std::pair<Undo, bool> Assign(Box box, int value);
  void UnAssign(const Undo& undo);

  void AddFilter(SolutionFilter solution_filter, std::vector<Box> input);

 private:
  friend class AllowedValueAdvancer;
  int CheckAllowed(SolutionFilter filter, Box box) const;

  // {id: {class: allowed_value_bv}}
  std::vector<std::vector<int>> bv_;
  std::vector<std::vector<bool>> assigned_;
  std::vector<std::vector<int>> vals_;
  std::vector<
      std::vector<std::vector<std::pair<SolutionFilter, std::vector<Box>>>>>
      solution_filters_;
  MutableSolution* mutable_solution_;
  Solution testable_solution_;
};

class AllowedValueSolutionPermuter final : public SolutionPermuter {
 public:
  explicit AllowedValueSolutionPermuter(const EntryDescriptor* e);
  ~AllowedValueSolutionPermuter() = default;

  // Movable, but not copyable.
  AllowedValueSolutionPermuter(const AllowedValueSolutionPermuter&) = delete;
  AllowedValueSolutionPermuter& operator=(const AllowedValueSolutionPermuter&) =
      delete;
  AllowedValueSolutionPermuter(AllowedValueSolutionPermuter&&) = default;
  AllowedValueSolutionPermuter& operator=(AllowedValueSolutionPermuter&&) =
      default;

  absl::Status PrepareCheap() override;
  absl::Status PrepareFull() override;

  absl::StatusOr<bool> AddFilter(SolutionFilter solution_filter) override;

  double Selectivity() const override { return 1.0; }

  iterator begin() const override;

  double permutation_count() const;

  const std::vector<std::pair<SolutionFilter, std::vector<AllowedValueGrid::Box>>>& solution_filters() const {
    return solution_filters_;
  }

 private:
  std::vector<std::pair<SolutionFilter, std::vector<AllowedValueGrid::Box>>> solution_filters_;
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

#endif  // PUZZLE_SOLUTION_PERMUTER_ALLOWED_VALUE_SOLUTION_PERMUTER_H