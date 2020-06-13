#ifndef SUDOKU_KILLER_SUDOKU_H
#define SUDOKU_KILLER_SUDOKU_H

#include <vector>

#include "sudoku/base.h"

namespace sudoku {

/*
Killer sudoku are problems where the initial constraints are mathematical
constraints on sets of values (like sums) rather than some initial values.
 */
class KillerSudoku : public ::sudoku::Base {
 public:
  struct Box {
    int entry_id;
    int class_id;

    std::string DebugString() const {
      return absl::StrCat("(", entry_id, ", ", class_id, ")");
    }

    template <typename H>
    friend H AbslHashValue(H h, const Box& box) {
      return H::combine(std::move(h), box.entry_id, box.class_id);
    }

    bool operator==(const Box& other) const {
      return entry_id == other.entry_id && class_id == other.class_id;
    }
  };

  struct Cage {
    int expected_sum;
    std::vector<Box> boxes;
  };

  Board GetInstanceBoard() const override;
  Board GetSolutionBoard() const override;

  std::vector<Cage> GetCages() const;

 protected:
  void InstanceSetup() override;

 private:
  void AddCage(const Cage& cage);

  absl::flat_hash_set<Box> box_used_;
};

std::ostream& operator<<(std::ostream& out, const KillerSudoku::Box& box) {
  return out << box.DebugString();
}

}  // namespace sudoku

#endif  // SUDOKU_KILLER_SUDOKU_H
