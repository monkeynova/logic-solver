#ifndef SUDOKU_GREATER_THAN_SUDOKU_H
#define SUDOKU_GREATER_THAN_SUDOKU_H

#include <vector>

#include "sudoku/base.h"

namespace sudoku {

/*
GreaterThan sudoku are problems where the initial constraints are inequalitiy
relations on pairs of values rather than some initial values.
 */
class GreaterThanSudoku : public ::sudoku::Base {
 public:
  Board GetInstanceBoard() const override { return Board(); }

  virtual std::vector<std::pair<Box, Box>> GetComparisons() const = 0;

 protected:
  absl::Status InstanceSetup() override;

 private:
  absl::Status AddComparison(const std::pair<Box, Box>& comparison);
};

}  // namespace sudoku

#endif  // SUDOKU_GREATER_THAN_SUDOKU_H
