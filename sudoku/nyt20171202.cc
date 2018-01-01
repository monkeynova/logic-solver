#include "sudoku/base.h"

namespace sudoku {

class NYT20171202 : public Base {
  puzzle::Solution Solution() const override;
  void AddInstancePredicates() override;
};

void NYT20171202::AddInstancePredicates() {
  /*
    8 ? 5 | ? ? ? | ? 3 9
    ? ? ? | ? ? ? | ? ? ?
    ? 3 9 | 5 ? ? | ? ? 4
    - - - + - - - + - - -
    2 ? ? | ? 7 ? | ? ? 8
    ? ? ? | ? 2 ? | 9 5 ?
    ? ? 4 | ? ? ? | ? ? 1
    - - - + - - - + - - -
    9 ? ? | ? ? 8 | 7 ? ?
    ? ? ? | ? ? 3 | 6 ? ?
    ? ? 2 | 9 1 ? | ? ? ?
  */
  AddValuePredicate(1, 1, 8);
  AddValuePredicate(1, 3, 5);
  AddValuePredicate(1, 8, 3);
  AddValuePredicate(1, 9, 9);
  AddValuePredicate(3, 2, 3);
  AddValuePredicate(3, 3, 9);
  AddValuePredicate(3, 4, 5);
  AddValuePredicate(3, 9, 4);
  AddValuePredicate(4, 1, 2);
  AddValuePredicate(4, 5, 7);
  AddValuePredicate(4, 9, 8);
  AddValuePredicate(5, 5, 2);
  AddValuePredicate(5, 7, 9);
  AddValuePredicate(5, 8, 5);
  AddValuePredicate(6, 3, 4);
  AddValuePredicate(6, 9, 1);
  AddValuePredicate(7, 1, 9);
  AddValuePredicate(7, 6, 8);
  AddValuePredicate(7, 7, 7);
  AddValuePredicate(8, 6, 3);
  AddValuePredicate(8, 7, 6);
  AddValuePredicate(9, 3, 2);
  AddValuePredicate(9, 4, 9);
  AddValuePredicate(9, 5, 1);
}

puzzle::Solution NYT20171202::Solution() const {
  /*
I1231 16:52:05.938428 2819318592 puzzle_main.cc:44] 0: 1=8 2=1 3=5 4=7 5=6 6=4 7=2 8=3 9=9
1: 1=4 2=2 3=7 4=1 5=3 6=9 7=5 8=8 9=6
2: 1=6 2=3 3=9 4=5 5=8 6=2 7=1 8=7 9=4
3: 1=2 2=9 3=1 4=3 5=7 6=5 7=4 8=6 9=8
4: 1=3 2=8 3=6 4=4 5=2 6=1 7=9 8=5 9=7
5: 1=7 2=5 3=4 4=8 5=9 6=6 7=3 8=2 9=1
6: 1=9 2=4 3=3 4=6 5=5 6=8 7=7 8=1 9=2
7: 1=1 2=7 3=8 4=2 5=4 6=3 7=6 8=9 9=5
8: 1=5 2=6 3=2 4=9 5=1 6=7 7=8 8=4 9=3
I1231 16:52:05.938474 2819318592 puzzle_main.cc:48] [1 solutions tested in 551.509s]

    8 1 5 | 7 6 4 | 2 3 9
    4 2 7 | 1 3 9 | 5 8 6
    6 3 9 | 5 8 2 | 1 7 4
    - - - + - - - + - - -
    2 9 1 | 3 7 5 | 4 6 8
    3 8 6 | 4 2 1 | 9 5 7
    7 5 4 | 8 9 6 | 3 2 1
    - - - + - - - + - - -
    9 4 3 | 6 5 8 | 7 1 2
    1 7 8 | 2 4 3 | 6 9 5
    5 6 2 | 9 1 7 | 8 4 3
  */
  std::vector<puzzle::Entry> entries;
  entries.emplace_back(0, std::vector<int>{8, 1, 5, 7, 6, 4 , 2, 3, 9},
                       entry_descriptor());
  entries.emplace_back(1, std::vector<int>{4, 2, 7, 1, 3, 9 , 5, 8, 6},
                       entry_descriptor());
  entries.emplace_back(2, std::vector<int>{6, 3, 9, 5, 8, 2 , 1, 7, 4},
                       entry_descriptor());
  entries.emplace_back(3, std::vector<int>{2, 9, 1, 3, 7, 5 , 4, 6, 8},
                       entry_descriptor());
  entries.emplace_back(4, std::vector<int>{3, 8, 6, 4, 2, 1 , 9, 5, 7},
                       entry_descriptor());
  entries.emplace_back(5, std::vector<int>{7, 5, 4, 8, 9, 6 , 3, 2, 1},
                       entry_descriptor());
  entries.emplace_back(6, std::vector<int>{9, 4, 3, 6, 5, 8 , 7, 1, 2},
                       entry_descriptor());
  entries.emplace_back(7, std::vector<int>{1, 7, 8, 2, 4, 3 , 6, 9, 5},
                       entry_descriptor());
  entries.emplace_back(8, std::vector<int>{5, 6, 2, 9, 1, 7 , 8, 4, 3},
                       entry_descriptor());

  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20171202);
