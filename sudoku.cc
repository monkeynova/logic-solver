/*
Logic solver repurposed for sudoku
 */
#include <iostream>
#include <memory>

#include "absl/memory/memory.h"
#include "puzzle/solver.h"

void SetupProblem(
    Puzzle::Solver* s,
    std::vector<std::unique_ptr<Puzzle::Descriptor>> *descriptors) {
  auto row_descriptor = absl::make_unique<Puzzle::IntRangeDescriptor>(0, 8);
  auto val_descriptor = absl::make_unique<Puzzle::IntRangeDescriptor>(1, 9);

  s->SetIdentifiers(row_descriptor.get());
  for (int i = 0; i < 9; ++i) {
    s->AddClass(i, absl::StrCat(i + 1), val_descriptor.get());
  }

  descriptors->push_back(std::move(row_descriptor));
  descriptors->push_back(std::move(val_descriptor));
}

void AddProblemPredicates(Puzzle::Solver* s) {
  for (int i = 0; i < 9; ++i) {
    s->AddPredicate(absl::StrCat("No row dupes ", i + 1),
                    [i](const Puzzle::Entry& e) {
                      for (int j = 0; j < i; ++j) {
                        if (e.Class(i) == e.Class(j))
                          return false;
                      }
                      return true;
                    },
                    i);
  }

  for (int i = 0; i < 9; ++i) {
    if (i % 3 == 0) continue;
     
    s->AddPredicate(
        absl::StrCat("No box dupes ", i + 1),
        [i](const Puzzle::Solution& s) {
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
        i);
  }
}

static void AddValuePredicate(int row, int col, int value, Puzzle::Solver* s) {
  s->AddPredicate(absl::StrCat("(", row, ",", col, ") = ", value),
                  [row, col, value](const Puzzle::Solution& s) {
                    return s.Id(row - 1).Class(col - 1) == value;
                  },
                  col - 1);
}

void AddRulePredicates(Puzzle::Solver* s) {
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
