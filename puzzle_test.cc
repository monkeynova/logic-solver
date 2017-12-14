#include <iostream>
#include <memory>
#include <vector>

#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solver.h"

extern void SetupProblem(puzzle::Solver* s);
extern puzzle::Solution ProblemSolution(const puzzle::Solver& s);

TEST(Puzzle, RightAnswer) {
  puzzle::Solver solver;
  
  SetupProblem(&solver);

  puzzle::Solution got = solver.Solve();
  puzzle::Solution expect = ProblemSolution(solver);
  
  EXPECT_EQ(got, expect) 
      << got.ToStr() << std::endl << expect.ToStr();
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
