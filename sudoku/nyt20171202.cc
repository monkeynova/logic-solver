#include "sudoku/base.h"

namespace sudoku {

class NYT20171202 : public Base {
 private:
  Board GetInstanceBoard() const override;
  Board GetSolutionBoard() const override;
};

Base::Board NYT20171202::GetInstanceBoard() const {
  return ParseBoard(
      "8 ? 5 | ? ? ? | ? 3 9\n"
      "? ? ? | ? ? ? | ? ? ?\n"
      "? 3 9 | 5 ? ? | ? ? 4\n"
      "- - - + - - - + - - -\n"
      "2 ? ? | ? 7 ? | ? ? 8\n"
      "? ? ? | ? 2 ? | 9 5 ?\n"
      "? ? 4 | ? ? ? | ? ? 1\n"
      "- - - + - - - + - - -\n"
      "9 ? ? | ? ? 8 | 7 ? ?\n"
      "? ? ? | ? ? 3 | 6 ? ?\n"
      "? ? 2 | 9 1 ? | ? ? ?");
}

Base::Board NYT20171202::GetSolutionBoard() const {
  /*
I1231 16:52:05.938428 2819318592 puzzle_main.cc:44] 0: 1=8 2=1 3=5 4=7 5=6 6=4
7=2 8=3 9=9 1: 1=4 2=2 3=7 4=1 5=3 6=9 7=5 8=8 9=6 2: 1=6 2=3 3=9 4=5 5=8 6=2
7=1 8=7 9=4 3: 1=2 2=9 3=1 4=3 5=7 6=5 7=4 8=6 9=8 4: 1=3 2=8 3=6 4=4 5=2 6=1
7=9 8=5 9=7 5: 1=7 2=5 3=4 4=8 5=9 6=6 7=3 8=2 9=1 6: 1=9 2=4 3=3 4=6 5=5 6=8
7=7 8=1 9=2 7: 1=1 2=7 3=8 4=2 5=4 6=3 7=6 8=9 9=5 8: 1=5 2=6 3=2 4=9 5=1 6=7
7=8 8=4 9=3 I1231 16:52:05.938474 2819318592 puzzle_main.cc:48] [1 solutions
tested in 551.509s]*/

  return ParseBoard(
      "8 1 5 | 7 6 4 | 2 3 9\n"
      "4 2 7 | 1 3 9 | 5 8 6\n"
      "6 3 9 | 5 8 2 | 1 7 4\n"
      "- - - + - - - + - - -\n"
      "2 9 1 | 3 7 5 | 4 6 8\n"
      "3 8 6 | 4 2 1 | 9 5 7\n"
      "7 5 4 | 8 9 6 | 3 2 1\n"
      "- - - + - - - + - - -\n"
      "9 4 3 | 6 5 8 | 7 1 2\n"
      "1 7 8 | 2 4 3 | 6 9 5\n"
      "5 6 2 | 9 1 7 | 8 4 3");
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20171202);
