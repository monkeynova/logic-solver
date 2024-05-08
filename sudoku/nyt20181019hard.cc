#include "sudoku/sudoku.h"

namespace sudoku {

class NYT20181019Hard : public Sudoku {
 private:
  absl::StatusOr<Board> GetInstanceBoard() const override;
  absl::StatusOr<Board> GetSolutionBoard() const override;
};

absl::StatusOr<Sudoku::Board> NYT20181019Hard::GetInstanceBoard() const {
  return ParseBoard(
      "? ? ? | 1 ? ? | ? ? 3\n"
      "? ? ? | ? ? ? | 4 5 ?\n"
      "? 8 2 | 9 3 ? | ? ? ?\n"
      "- - - + - - - + - - -\n"
      "? 2 ? | 8 ? 3 | ? ? ?\n"
      "? ? 4 | ? ? ? | 6 ? ?\n"
      "? ? 1 | ? ? ? | ? ? 9\n"
      "- - - + - - - + - - -\n"
      "? 1 ? | ? ? 6 | ? 7 ?\n"
      "6 ? ? | ? 5 8 | ? ? ?\n"
      "? ? 3 | ? ? ? | ? ? 2");
}

absl::StatusOr<Sudoku::Board> NYT20181019Hard::GetSolutionBoard() const {
  /*
I1019 16:34:49.435868 2793841472 puzzle_main.cc:44] 0: 1=7 2=6 3=5 4=1 5=8 6=4
7=2 8=9 9=3 1: 1=1 2=3 3=9 4=6 5=7 6=2 7=4 8=5 9=8 2: 1=4 2=8 3=2 4=9 5=3 6=5
7=7 8=1 9=6 3: 1=9 2=2 3=6 4=8 5=1 6=3 7=5 8=4 9=7 4: 1=3 2=7 3=4 4=5 5=2 6=9
7=6 8=8 9=1 5: 1=8 2=5 3=1 4=4 5=6 6=7 7=3 8=2 9=9 6: 1=2 2=1 3=8 4=3 5=4 6=6
7=9 8=7 9=5 7: 1=6 2=9 3=7 4=2 5=5 6=8 7=1 8=3 9=4 8: 1=5 2=4 3=3 4=7 5=9 6=1
7=8 8=6 9=2 I1019 16:34:49.435915 2793841472 puzzle_main.cc:48] [1 solutions
tested in 1.39182s]
   */
  return ParseBoard(
      "7 6 5 | 1 8 4 | 2 9 3\n"
      "1 3 9 | 6 7 2 | 4 5 8\n"
      "4 8 2 | 9 3 5 | 7 1 6\n"
      "- - - + - - - + - - -\n"
      "9 2 6 | 8 1 3 | 5 4 7\n"
      "3 7 4 | 5 2 9 | 6 8 1\n"
      "8 5 1 | 4 6 7 | 3 2 9\n"
      "- - - + - - - + - - -\n"
      "2 1 8 | 3 4 6 | 9 7 5\n"
      "6 9 7 | 2 5 8 | 1 3 4\n"
      "5 4 3 | 7 9 1 | 8 6 2");
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20181019Hard);
