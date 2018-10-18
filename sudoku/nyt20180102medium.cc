#include "sudoku/base.h"

namespace sudoku {

class NYT20180102Medium : public Base {
  Board GetInstanceBoard() const override;
  Board GetSolutionBoard() const override;
};

Base::Board NYT20180102Medium::GetInstanceBoard() const {
  return ParseBoard(
    "? 9 ? | ? ? 3 | ? 1 2\n"
    "5 ? ? | ? ? ? | ? ? ?\n"
    "? ? ? | ? ? ? | 3 9 ?\n"
    "- - - + - - - + - - -\n"
    "? ? ? | 3 ? ? | 9 ? 4\n"
    "? 6 7 | ? ? ? | ? ? ?\n"
    "2 ? ? | 6 ? 8 | ? 3 ?\n"
    "- - - + - - - + - - -\n"
    "? 1 ? | 2 ? 6 | ? ? ?\n"
    "? ? 8 | ? ? ? | ? ? ?\n"
    "? 3 ? | 4 5 ? | ? ? 6");
}

Base::Board NYT20180102Medium::GetSolutionBoard() const {
  /*
I1017 17:48:58.072470 2793841472 puzzle_main.cc:44] 0: 1=8 2=9 3=6 4=5 5=7 6=3 7=4 8=1 9=2
1: 1=5 2=2 3=3 4=1 5=9 6=4 7=6 8=7 9=8
2: 1=4 2=7 3=1 4=8 5=6 6=2 7=3 8=9 9=5
3: 1=1 2=8 3=5 4=3 5=2 6=7 7=9 8=6 9=4
4: 1=3 2=6 3=7 4=9 5=4 6=5 7=8 8=2 9=1
5: 1=2 2=4 3=9 4=6 5=1 6=8 7=5 8=3 9=7
6: 1=9 2=1 3=4 4=2 5=8 6=6 7=7 8=5 9=3
7: 1=6 2=5 3=8 4=7 5=3 6=1 7=2 8=4 9=9
8: 1=7 2=3 3=2 4=4 5=5 6=9 7=1 8=8 9=6
I1017 17:48:58.072588 2793841472 puzzle_main.cc:48] [1 solutions tested in 31.712s]
  */
  return ParseBoard(
    "8 9 6 | 5 7 3 | 4 1 2\n"
    "5 2 3 | 1 9 4 | 6 7 8\n"
    "4 7 1 | 8 6 2 | 3 9 5\n"
    "- - - + - - - + - - -\n"
    "1 8 5 | 3 2 7 | 9 6 4\n"
    "3 6 7 | 9 4 5 | 8 2 1\n"
    "2 4 9 | 6 1 8 | 5 3 7\n"
    "- - - + - - - + - - -\n"
    "9 1 4 | 2 8 6 | 7 5 3\n"
    "6 5 8 | 7 3 1 | 2 4 9\n"
    "7 3 2 | 4 5 9 | 1 8 6");
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20180102Medium);
