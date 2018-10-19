#include "sudoku/base.h"

namespace sudoku {

class NYT20181019Easy : public Base {
 private:
  Board GetInstanceBoard() const override;
  Board GetSolutionBoard() const override;
};

Base::Board NYT20181019Easy::GetInstanceBoard() const {
  return ParseBoard(
    "3 ? 8 | ? ? ? | ? 1 6\n"
    "2 ? 1 | 7 5 6 | ? ? ?\n"
    "? ? 6 | ? ? 1 | 9 7 2\n"
    "- - - + - - - + - - -\n"
    "? 3 ? | 4 1 ? | 2 ? ?\n"
    "? 8 9 | ? ? ? | 1 ? ?\n"
    "1 5 ? | 3 8 ? | ? ? 4\n"
    "- - - + - - - + - - -\n"
    "7 6 ? | 5 ? ? | ? 2 ?\n"
    "? ? ? | 6 4 2 | ? 8 5\n"
    "? ? 5 | ? ? ? | ? 6 9");
}

Base::Board NYT20181019Easy::GetSolutionBoard() const {
  /*
I1019 16:28:47.993701 2793841472 puzzle_main.cc:42] [position=2.97066e+49/1.09111e+50 (0.272261)]
I1019 16:28:47.993710 2793841472 puzzle_main.cc:44] 0: 1=3 2=7 3=8 4=9 5=2 6=4 7=5 8=1 9=6
1: 1=2 2=9 3=1 4=7 5=5 6=6 7=8 8=4 9=3
2: 1=5 2=4 3=6 4=8 5=3 6=1 7=9 8=7 9=2
3: 1=6 2=3 3=7 4=4 5=1 6=9 7=2 8=5 9=8
4: 1=4 2=8 3=9 4=2 5=6 6=5 7=1 8=3 9=7
5: 1=1 2=5 3=2 4=3 5=8 6=7 7=6 8=9 9=4
6: 1=7 2=6 3=4 4=5 5=9 6=8 7=3 8=2 9=1
7: 1=9 2=1 3=3 4=6 5=4 6=2 7=7 8=8 9=5
8: 1=8 2=2 3=5 4=1 5=7 6=3 7=4 8=6 9=9
I1019 16:28:47.993742 2793841472 puzzle_main.cc:48] [1 solutions tested in 0.45829s]
   */
  return ParseBoard(
    "3 7 8 | 9 2 4 | 5 1 6\n"
    "2 9 1 | 7 5 6 | 8 4 3\n"
    "5 4 6 | 8 3 1 | 9 7 2\n"
    "- - - + - - - + - - -\n"
    "6 3 7 | 4 1 9 | 2 5 8\n"
    "4 8 9 | 2 6 5 | 1 3 7\n"
    "1 5 2 | 3 8 7 | 6 9 4\n"
    "- - - + - - - + - - -\n"
    "7 6 4 | 5 9 8 | 3 2 1\n"
    "9 1 3 | 6 4 2 | 7 8 5\n"
    "8 2 5 | 1 7 3 | 4 6 9");
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20181019Easy);
