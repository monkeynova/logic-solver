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
 public:
  using Board = std::vector<std::vector<int>>;

  static Board ParseBoard(const std::string& board);

  virtual Board GetInstanceBoard() const = 0;
  virtual Board GetSolutionBoard() const = 0;
  
 private:
  // ::puzzle::Problem methods. Final to prevent missing parts.
  void Setup() final;
  ::puzzle::Solution GetSolution() const final;

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  void AddValuePredicate(int row, int col, int value);
  void AddBoardPredicates(const Board& board);
  void AddPredicates();
  void AddPredicatesCumulative();
  void AddPredicatesPairwise();
};

}  // namespace sudoku

#endif  // SUDOKU_BASE_H
