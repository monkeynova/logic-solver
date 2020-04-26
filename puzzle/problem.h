#ifndef PUZZLE_PROBLEM_H
#define PUZZLE_PROBLEM_H

#include <memory>

#include "puzzle/solver.h"

namespace puzzle {

class Problem : public Solver {
 public:
  using Generator = std::unique_ptr<Problem>(*)();
  virtual ~Problem() = default;

  virtual void Setup() = 0;
  virtual Solution GetSolution() const = 0;

  static std::unique_ptr<Problem> GetInstance();
  static Generator SetGenerator(Generator generator);

 private:
  static std::unique_ptr<Problem> MissingGenerator();
  
  static Generator global_problem_generator_;
};

}  // namespace puzzle

#define REGISTER_PROBLEM(Class)                                 \
  static std::unique_ptr<::puzzle::Problem> GeneratePuzzle() {  \
    return absl::make_unique<Class>();                          \
  }                                                             \
  static ::puzzle::Problem::Generator generator =               \
    ::puzzle::Problem::SetGenerator(&GeneratePuzzle); 

#endif  // PUZZLE_PROBLEM_H
