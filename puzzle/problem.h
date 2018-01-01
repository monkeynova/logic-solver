#ifndef PUZZLE_PROBLEM_H
#define PUZZLE_PROBLEM_H

#include "puzzle/solver.h"

namespace puzzle {

class Problem : public Solver {
 public:
  virtual ~Problem() = default;

  virtual void Setup() = 0;
  virtual Solution Solution() const = 0;

  static Problem* GetInstance();
};

}  // namespace puzzle

#define REGISTER_PROBLEM(Class)				\
  puzzle::Problem* puzzle::Problem::GetInstance() {	\
      static Class instance;				\
      return &instance;					\
  }

#endif  // PUZZLE_PROBLEM_H
