#include <iostream>
#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solver.h"

extern void SetupProblem(Puzzle::Solver* s);
extern void AddProblemPredicates(Puzzle::Solver* s);
extern void AddRulePredicates(Puzzle::Solver* s);

extern Puzzle::Solution ProblemSolution(const Puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  Puzzle::Solver solver;
  
  SetupProblem(&solver);
  AddProblemPredicates(&solver);
  AddRulePredicates(&solver);

  Puzzle::Solution got = solver.Solve();
  Puzzle::Solution expect = ProblemSolution(solver);
  
  EXPECT_EQ(got, expect) 
      << got.ToStr() << std::endl << expect.ToStr();
}
