#ifndef SUDOKU_H
#define SUDOKU_H

#include "puzzle/problem.h"

/*
Logic solver repurposed for sudoku
 */
class SudokuProblem : public puzzle::Problem {
 protected:
  void AddValuePredicate(int row, int col, int value);

 private:
  void Setup() override;
  puzzle::Solution Solution() const override { return puzzle::Solution(); }

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  void AddPredicates();
  void AddPredicatesCumulative();
  void AddPredicatesPairwise();

  virtual void AddInstancePredicates() = 0;
};

#endif  // SUDOKU_H
