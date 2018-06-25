#include "sudoku/base.h"

namespace sudoku {

class NYT20180101Easy : public Base {
  puzzle::Solution GetSolution() const override;
  void AddInstancePredicates() override;
};

void NYT20180101Easy::AddInstancePredicates() {
  AddBoardPredicates(ParseBoard(
    "2 1 ? | ? ? ? | 4 8 7\n"
    "8 ? ? | 3 ? 2 | ? 9 1\n"
    "9 ? 5 | ? 7 1 | ? ? ?\n"
    "- - - + - - - + - - -\n"
    "? ? 7 | 5 9 ? | 6 1 ?\n"
    "5 6 ? | ? ? 3 | ? ? 2\n"
    "4 ? 1 | 6 ? ? | 7 ? ?\n"
    "- - - + - - - + - - -\n"
    "? 3 9 | ? ? 7 | ? ? ?\n"
    "7 ? ? | 1 ? ? | ? 2 6\n"
    "1 ? ? | ? 6 5 | ? ? 9"));

  return;
}

puzzle::Solution NYT20180101Easy::GetSolution() const {
  /*
I0101 15:48:11.334648 2819318592 puzzle_main.cc:43] 0: 1=2 2=1 3=3 4=9 5=5 6=6 7=4 8=8 9=7
1: 1=8 2=7 3=6 4=3 5=4 6=2 7=5 8=9 9=1
2: 1=9 2=4 3=5 4=8 5=7 6=1 7=2 8=6 9=3
3: 1=3 2=2 3=7 4=5 5=9 6=4 7=6 8=1 9=8
4: 1=5 2=6 3=8 4=7 5=1 6=3 7=9 8=4 9=2
5: 1=4 2=9 3=1 4=6 5=2 6=8 7=7 8=3 9=5
6: 1=6 2=3 3=9 4=2 5=8 6=7 7=1 8=5 9=4
7: 1=7 2=5 3=4 4=1 5=3 6=9 7=8 8=2 9=6
8: 1=1 2=8 3=2 4=4 5=6 6=5 7=3 8=7 9=9
I0101 15:48:11.334724 2819318592 puzzle_main.cc:47] [1 solutions tested in 0.879936s]
  */
  return MakeSolution(ParseBoard(
    "2 1 3 | 9 5 6 | 4 8 7\n"
    "8 7 6 | 3 4 2 | 5 9 1\n"
    "9 4 5 | 8 7 1 | 2 6 3\n"
    "- - - + - - - + - - -\n"
    "3 2 7 | 5 9 4 | 6 1 8\n"
    "5 6 8 | 7 1 3 | 9 4 2\n"
    "4 9 1 | 6 2 8 | 7 3 5\n"
    "- - - + - - - + - - -\n"
    "6 3 9 | 2 8 7 | 1 5 4\n"
    "7 5 4 | 1 3 9 | 8 2 6\n"
    "1 8 2 | 4 6 5 | 3 7 9"));
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20180101Easy);
