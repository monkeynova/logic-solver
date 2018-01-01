#include "sudoku.h"

#include <iostream>
#include <memory>
#include <vector>

#include "absl/memory/memory.h"
#include "gflags/gflags.h"

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

void SudokuProblem::AddPredicatesCumulative() {
  std::vector<int> cols = {0};
  for (int i = 1; i < 9; ++i) {
    cols.push_back(i);
    AddPredicate(absl::StrCat("No row dupes ", i + 1),
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
    AddPredicate(
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

void SudokuProblem::AddPredicatesPairwise() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (i < j) {
        AddPredicate(absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
                     [i, j](const puzzle::Entry& e) {
                       return e.Class(i) != e.Class(j);
                     },
                     {i, j});
      }
    }
  }

  for (int box_base_x = 0; box_base_x < 9; box_base_x += 3) {
    for (int box_base_y = 0; box_base_y < 9; box_base_y += 3) {
      AddPredicate(
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

void SudokuProblem::AddValuePredicate(int row, int col, int value) {
  AddPredicate(absl::StrCat("(", row, ",", col, ") = ", value),
               [row, col, value](const puzzle::Solution& s) {
                 return s.Id(row - 1).Class(col - 1) == value;
               },
               col - 1);
}

void SudokuProblem::Setup() {
  puzzle::Descriptor* val_descriptor = AddDescriptor(
      new puzzle::IntRangeDescriptor(1, 9));

  SetIdentifiers(AddDescriptor(new puzzle::IntRangeDescriptor(0, 8)));
  for (int i = 0; i < 9; ++i) {
    AddClass(i, absl::StrCat(i + 1), val_descriptor);
  }

  if (FLAGS_sudoku_problem_setup == "cumulative") {
    AddPredicatesCumulative();
  } else if (FLAGS_sudoku_problem_setup == "pairwise") {
    AddPredicatesPairwise();
  } else {
    LOG(FATAL) << "Unrecognized option for sudoku_problem_setup '"
               << FLAGS_sudoku_problem_setup << "'; valid values are "
               << "'cumulative' and 'pairwise'.";
  }

  if (FLAGS_sudoku_setup_only) {
    return;
  }

  AddInstancePredicates();
}
