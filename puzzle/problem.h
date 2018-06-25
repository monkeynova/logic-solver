#ifndef PUZZLE_PROBLEM_H
#define PUZZLE_PROBLEM_H

#include "puzzle/solver.h"

namespace puzzle {

class Problem : public Solver {
 public:
  virtual ~Problem() = default;

  virtual void Setup() = 0;
  virtual Solution GetSolution() const = 0;

  static Problem* GetInstance();
  static Problem* SetInstance(Problem* p);

 private:
  static Problem* global_instance_;
};

}  // namespace puzzle

#define REGISTER_PROBLEM(Class)                      \
  ::puzzle::Problem* global_problem =                \
      ::puzzle::Problem::SetInstance(new Class());

#endif  // PUZZLE_PROBLEM_H
