#ifndef SUDOKU_BASE_H
#define SUDOKU_BASE_H

#include <string>
#include <vector>

#include "puzzle/problem.h"

namespace sudoku {

/*
Logic solver repurposed for sudoku
 */
class Base : public ::puzzle::Problem {
 protected:
  using Board = std::vector<std::vector<int>>;

  void AddValuePredicate(int row, int col, int value);

  static Board ParseBoard(const std::string& board);
  void AddBoardPredicates(const Board& board);
  ::puzzle::Solution MakeSolution(const Board& board) const;

 private:
  void Setup() override;
  ::puzzle::Solution GetSolution() const override { return puzzle::Solution(); }

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  void AddPredicates();
  void AddPredicatesCumulative();
  void AddPredicatesPairwise();

  virtual void AddInstancePredicates() = 0;
};

}  // namespace sudoku

#endif  // SUDOKU_BASE_H
