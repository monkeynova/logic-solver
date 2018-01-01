#ifndef SUDOKU_BASE_H
#define SUDOKU_BASE_H

#include "puzzle/problem.h"

namespace sudoku {

/*
Logic solver repurposed for sudoku
 */
class Base : public puzzle::Problem {
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

}  // namespace sudoku

#endif  // SUDOKU_BASE_H
