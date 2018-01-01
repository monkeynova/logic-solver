/*
Logic solver repurposed for sudoku
 */
#include <iostream>
#include <memory>
#include <vector>

#include "absl/memory/memory.h"
#include "gflags/gflags.h"
#include "puzzle/solver.h"

DEFINE_string(sudoku_problem_setup, "cumulative",
	      "Sepecifies the form of the predicates passed to the puzzle "
	      "solver to validate sudoku boards. Valid vaules are 'cumulative' "
	      "and 'pairwise'. 'cumulative' is faster if predicate reordering "
	      "is disabled, but 'pairwise' is better suited for predicate "
	      "reordering and results in faster overall evaluation if "
	      "reordering is enabled.");

DEFINE_bool(sudoku_setup_only, false,
            "If true, only set up predicates for valid sudoku board "
            "configuration rather than solving a specific board.");

static void AddProblemPredicatesCumulative(puzzle::Solver* s) {
  std::vector<int> cols = {0};
  for (int i = 1; i < 9; ++i) {
    cols.push_back(i);
    s->AddPredicate(absl::StrCat("No row dupes ", i + 1),
                    [i](const puzzle::Entry& e) {
                      for (int j = 0; j < i; ++j) {
                        if (e.Class(i) == e.Class(j)) return false;
                      }
                      return true;
                    },
                    cols);
  }

  for (int i = 0; i < 9; ++i) {
    if (i % 3 == 0) {
      cols = {0};
      continue;
    }

    cols.push_back(i);
    s->AddPredicate(
        absl::StrCat("No box dupes ", i + 1),
        [i](const puzzle::Solution& s) {
          for (int row = 0; row < 9; ++row) {
            int to_match = s.Id(row).Class(i);

            int box_base = row - (row % 3);
            for (int col = i - (i % 3); col < i; ++col) {
              for (int box_row = 0; box_row < 3; ++box_row) {
                if (s.Id(box_base + box_row).Class(col) == to_match)
                  return false;
              }
            }
          }
          return true;
        },
        cols);
  }
}

static void AddProblemPredicatesPairwise(puzzle::Solver* s) {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (i < j) {
        s->AddPredicate(absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
                        [i, j](const puzzle::Entry& e) {
                          return e.Class(i) != e.Class(j);
                        },
                        {i, j});
      }
    }
  }

  for (int box_base_x = 0; box_base_x < 9; box_base_x += 3) {
    for (int box_base_y = 0; box_base_y < 9; box_base_y += 3) {
      s->AddPredicate(
          absl::StrCat("No box dupes (", box_base_x + 1, ",", box_base_y + 1,
                       ")"),
          [box_base_x, box_base_y](const puzzle::Solution& s) {
            int hist = 0;
            for (int i = 0; i < 3; ++i) {
              for (int j = 0; j < 3; ++j) {
                int bit = s.Id(box_base_x + i).Class(box_base_y + j);
                if (hist & (1<<bit)) return false;
                hist |= 1<<bit;
              }
            }
            return true;
          },
          {box_base_y, box_base_y + 1, box_base_y + 2});
    }
  }
}

static void AddValuePredicate(int row, int col, int value, puzzle::Solver* s) {
  s->AddPredicate(absl::StrCat("(", row, ",", col, ") = ", value),
                  [row, col, value](const puzzle::Solution& s) {
                    return s.Id(row - 1).Class(col - 1) == value;
                  },
                  col - 1);
}

void AddRulePredicates(puzzle::Solver* s) {
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
  AddValuePredicate(1, 1, 8, s);
  AddValuePredicate(1, 3, 5, s);
  AddValuePredicate(1, 8, 3, s);
  AddValuePredicate(1, 9, 9, s);
  AddValuePredicate(3, 2, 3, s);
  AddValuePredicate(3, 3, 9, s);
  AddValuePredicate(3, 4, 5, s);
  AddValuePredicate(3, 9, 4, s);
  AddValuePredicate(4, 1, 2, s);
  AddValuePredicate(4, 5, 7, s);
  AddValuePredicate(4, 9, 8, s);
  AddValuePredicate(5, 5, 2, s);
  AddValuePredicate(5, 7, 9, s);
  AddValuePredicate(5, 8, 5, s);
  AddValuePredicate(6, 3, 4, s);
  AddValuePredicate(6, 9, 1, s);
  AddValuePredicate(7, 1, 9, s);
  AddValuePredicate(7, 6, 8, s);
  AddValuePredicate(7, 7, 7, s);
  AddValuePredicate(8, 6, 3, s);
  AddValuePredicate(8, 7, 6, s);
  AddValuePredicate(9, 3, 2, s);
  AddValuePredicate(9, 4, 9, s);
  AddValuePredicate(9, 5, 1, s);
}

puzzle::Solution ProblemSolution(const puzzle::Solver& s) {
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
		       s.entry_descriptor());
  entries.emplace_back(1, std::vector<int>{4, 2, 7, 1, 3, 9 , 5, 8, 6},
		       s.entry_descriptor());
  entries.emplace_back(2, std::vector<int>{6, 3, 9, 5, 8, 2 , 1, 7, 4},
		       s.entry_descriptor());
  entries.emplace_back(3, std::vector<int>{2, 9, 1, 3, 7, 5 , 4, 6, 8},
		       s.entry_descriptor());
  entries.emplace_back(4, std::vector<int>{3, 8, 6, 4, 2, 1 , 9, 5, 7},
		       s.entry_descriptor());
  entries.emplace_back(5, std::vector<int>{7, 5, 4, 8, 9, 6 , 3, 2, 1},
		       s.entry_descriptor());
  entries.emplace_back(6, std::vector<int>{9, 4, 3, 6, 5, 8 , 7, 1, 2},
		       s.entry_descriptor());
  entries.emplace_back(7, std::vector<int>{1, 7, 8, 2, 4, 3 , 6, 9, 5},
		       s.entry_descriptor());
  entries.emplace_back(8, std::vector<int>{5, 6, 2, 9, 1, 7 , 8, 4, 3},
		       s.entry_descriptor());

  return puzzle::Solution(s.entry_descriptor(), &entries).Clone();
}

void SetupProblem(puzzle::Solver* s) {
  puzzle::Descriptor* val_descriptor = s->AddDescriptor(
      new puzzle::IntRangeDescriptor(1, 9));

  s->SetIdentifiers(s->AddDescriptor(
      new puzzle::IntRangeDescriptor(0, 8)));
  for (int i = 0; i < 9; ++i) {
    s->AddClass(i, absl::StrCat(i + 1), val_descriptor);
  }

  if (FLAGS_sudoku_problem_setup == "cumulative") {
    AddProblemPredicatesCumulative(s);
  } else if (FLAGS_sudoku_problem_setup == "pairwise") {
    AddProblemPredicatesPairwise(s);
  } else {
    LOG(FATAL) << "Unrecognized option for sudoku_problem_setup '"
	       << FLAGS_sudoku_problem_setup << "'; valid values are "
	       << "'cumulative' and 'pairwise'.";
  }
  
  if (FLAGS_sudoku_setup_only) {
    return;
  }

  AddRulePredicates(s);
}
