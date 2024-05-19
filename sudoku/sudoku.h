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
  static constexpr int kWidth = 9;
  static constexpr int kSubHeight = 3;
  static_assert(kWidth % kSubHeight == 0, "Sub box must fit");

  using Box = ::ken_ken::Grid<kWidth>::Box;
  using Board = ::ken_ken::Grid<kWidth>::Board;

  Sudoku() = default;

  static Board EmptyBoard() {
    Board ret;
    absl::c_for_each(ret, [](auto row) {
      absl::c_for_each(row, [](int& cell) { cell = 0; });
    });
    return ret;
  }

  static absl::StatusOr<Board> ParseBoard(const absl::string_view board);

  virtual absl::StatusOr<Board> GetInstanceBoard() const = 0;

 protected:
  virtual absl::Status InstanceSetup(::ken_ken::Grid<kWidth>::Orientation o);

 private:
  // ::puzzle::Problem methods. Final to prevent missing parts.
  absl::Status AddGridPredicates(::ken_ken::Grid<kWidth>::Orientation o) final;

  absl::Status AddValuePredicate(int row, int col, int value);
  absl::Status AddComposedValuePredicates(int row, int col, int value);
  absl::Status AddBoardPredicates(const Board& board);
  absl::Status AddPredicatesCumulative();
  absl::Status AddPredicatesPairwise();

  static puzzle::EntryDescriptor MakeEntryDescriptor();
};

}  // namespace sudoku

#endif  // SUDOKU_SUDOKU_H
