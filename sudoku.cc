/*
Logic solver repurposed for sudoku
 */
#include <iostream>
#include <memory>

#include "absl/memory/memory.h"
#include "gflags/gflags.h"
#include "puzzle/solver.h"

DEFINE_bool(sudoku_problem_setup_a, true,
	    "Is-valid-sudoku-board predicates have two forms which may have "
	    "different performance characteristics based on the details "
	    "of CroppedSolutionPermuter. This switches betweeen (a) and (b).");

DEFINE_bool(sudoku_setup_only, false,
	    "If true, only set up predicates for valid sudoku board "
	    "configuration rather than solving a specific board.");

static void AddProblemPredicatesSetupA(puzzle::Solver* s) {
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

static void AddProblemPredicatesSetupB(puzzle::Solver* s) {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (i != j) {
	s->AddPredicate(absl::StrCat("No row dupes ", i + 1),
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

void SetupProblem(puzzle::Solver* s) {
  puzzle::Descriptor* val_descriptor = s->AddDescriptor(
      new puzzle::IntRangeDescriptor(1, 9));

  s->SetIdentifiers(s->AddDescriptor(
      new puzzle::IntRangeDescriptor(0, 8)));
  for (int i = 0; i < 9; ++i) {
    s->AddClass(i, absl::StrCat(i + 1), val_descriptor);
  }

  if (FLAGS_sudoku_problem_setup_a) {
    AddProblemPredicatesSetupA(s);
  } else {
    AddProblemPredicatesSetupB(s);
  }
  
  if (FLAGS_sudoku_setup_only) {
    return;
  }

  AddRulePredicates(s);
}
