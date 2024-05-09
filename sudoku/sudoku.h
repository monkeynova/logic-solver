#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include <string>
#include <vector>

#include "ken_ken/grid.h"

namespace sudoku {

/*
Logic solver repurposed for sudoku
 */
class Sudoku : public ::ken_ken::Grid<9> {
 public:
  using Box = ::ken_ken::Grid<9>::Box;

  using Board = std::array<std::array<int, 9>, 9>;

  Sudoku() = default;

  static Board EmptyBoard() {
    return Board{std::array<int, 9>{0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 0, 0}};
  }

  static absl::StatusOr<Board> ParseBoard(const absl::string_view board);

  virtual absl::StatusOr<Board> GetInstanceBoard() const = 0;
  virtual absl::StatusOr<Board> GetSolutionBoard() const = 0;

 protected:
  virtual absl::Status InstanceSetup();

 private:
  // ::puzzle::Problem methods. Final to prevent missing parts.
  absl::Status AddGridPredicates() final;
  absl::StatusOr<::puzzle::Solution> GetSolution() const final;

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  absl::Status AddValuePredicate(int row, int col, int value);
  absl::Status AddComposedValuePredicates(int row, int col, int value);
  absl::Status AddBoardPredicates(const Board& board);
  absl::Status AddPredicatesCumulative();
  absl::Status AddPredicatesPairwise();
};

}  // namespace sudoku

#endif  // SUDOKU_SUDOKU_H
