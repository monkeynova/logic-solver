#ifndef SUDOKU_KILLER_SUDOKU_H
#define SUDOKU_KILLER_SUDOKU_H

#include <vector>

#include "sudoku/sudoku.h"

namespace sudoku {

/*
Killer sudoku are problems where the initial constraints are mathematical
constraints on sets of values (like sums) rather than some initial values.
 */
class KillerSudoku : public ::sudoku::Sudoku {
 public:
  struct Cage {
    int expected_sum;
    std::vector<Box> boxes;
  };

  absl::StatusOr<Board> GetInstanceBoard() const override { return Board(); }

  virtual std::vector<Cage> GetCages() const = 0;

 protected:
  absl::Status InstanceSetup() override;

 private:
  absl::Status AddCage(const Cage& cage);

  absl::flat_hash_set<Box> box_used_;
};

}  // namespace sudoku

#endif  // SUDOKU_KILLER_SUDOKU_H
