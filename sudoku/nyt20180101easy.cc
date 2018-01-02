#include "sudoku/base.h"

namespace sudoku {

class NYT20180101Easy : public Base {
  puzzle::Solution Solution() const override;
  void AddInstancePredicates() override;
};

void NYT20180101Easy::AddInstancePredicates() {
  /*
    2 1 ? | ? ? ? | 4 8 7
    8 ? ? | 3 ? 2 | ? 9 1
    9 ? 5 | ? 7 1 | ? ? ?
    - - - + - - - + - - -
    ? ? 7 | 5 9 ? | 6 1 ?
    5 6 ? | ? ? 3 | ? ? 2
    4 ? 1 | 6 ? ? | 7 ? ?
    - - - + - - - + - - -
    ? 3 9 | ? ? 7 | ? ? ?
    7 ? ? | 1 ? ? | ? 2 6
    1 ? ? | ? 6 5 | ? ? 9
  */
  AddValuePredicate(1, 1, 2);
  AddValuePredicate(1, 2, 1);
  AddValuePredicate(1, 7, 4);
  AddValuePredicate(1, 8, 8);
  AddValuePredicate(1, 9, 7);
  AddValuePredicate(2, 1, 8);
  AddValuePredicate(2, 4, 3);
  AddValuePredicate(2, 6, 2);
  AddValuePredicate(2, 8, 9);
  AddValuePredicate(2, 9, 1);
  AddValuePredicate(3, 1, 9);
  AddValuePredicate(3, 3, 5);
  AddValuePredicate(3, 5, 7);
  AddValuePredicate(3, 6, 1);
  AddValuePredicate(4, 3, 7);
  AddValuePredicate(4, 4, 5);
  AddValuePredicate(4, 5, 9);
  AddValuePredicate(4, 7, 6);
  AddValuePredicate(4, 8, 1);
  AddValuePredicate(5, 1, 5);
  AddValuePredicate(5, 2, 6);
  AddValuePredicate(5, 6, 3);
  AddValuePredicate(5, 9, 2);
  AddValuePredicate(6, 1, 4);
  AddValuePredicate(6, 3, 1);
  AddValuePredicate(6, 4, 6);
  AddValuePredicate(6, 7, 7);
  AddValuePredicate(7, 2, 3);
  AddValuePredicate(7, 3, 9);
  AddValuePredicate(7, 6, 7);
  AddValuePredicate(8, 1, 7);
  AddValuePredicate(8, 4, 1);
  AddValuePredicate(8, 8, 2);
  AddValuePredicate(8, 9, 6);
  AddValuePredicate(9, 1, 1);
  AddValuePredicate(9, 5, 6);
  AddValuePredicate(9, 6, 5);
  AddValuePredicate(9, 9, 9);
}

puzzle::Solution NYT20180101Easy::Solution() const {
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

    2 1 3 | 9 5 6 | 4 8 7
    8 7 6 | 3 4 2 | 5 9 1
    9 4 5 | 8 7 1 | 2 6 3
    - - - + - - - + - - -
    3 2 7 | 5 9 4 | 6 1 8
    5 6 8 | 7 1 3 | 9 4 2
    4 9 1 | 6 2 8 | 7 3 5
    - - - + - - - + - - -
    6 3 9 | 2 8 7 | 1 5 4
    7 5 4 | 1 3 9 | 8 2 6
    1 8 2 | 4 6 5 | 3 7 9
  */
  std::vector<puzzle::Entry> entries;
  entries.emplace_back(0, std::vector<int>{2, 1, 3, 9, 5, 6, 4, 8, 7},
                       entry_descriptor());
  entries.emplace_back(1, std::vector<int>{8, 7, 6, 3, 4, 2, 5, 9, 1},
                       entry_descriptor());
  entries.emplace_back(2, std::vector<int>{9, 4, 5, 8, 7, 1, 2, 6, 3},
                       entry_descriptor());
  entries.emplace_back(3, std::vector<int>{3, 2, 7, 5, 9, 4, 6, 1, 8},
                       entry_descriptor());
  entries.emplace_back(4, std::vector<int>{5, 6, 8, 7, 1, 3, 9, 4, 2},
                       entry_descriptor());
  entries.emplace_back(5, std::vector<int>{4, 9, 1, 6, 2, 8, 7, 3, 5},
                       entry_descriptor());
  entries.emplace_back(6, std::vector<int>{6, 3, 9, 2, 8, 7, 1, 5, 4},
                       entry_descriptor());
  entries.emplace_back(7, std::vector<int>{7, 5, 4, 1, 3, 9, 8, 2, 6},
                       entry_descriptor());
  entries.emplace_back(8, std::vector<int>{1, 8, 2, 4, 6, 5, 3, 7, 9},
                       entry_descriptor());

  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::NYT20180101Easy);
