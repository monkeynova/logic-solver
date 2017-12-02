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
  for (int i = 0; i < 8; ++i) {
    s->AddClass(i, absl::StrCat(i + 1), val_descriptor.get());
  }

  descriptors->push_back(std::move(row_descriptor));
  descriptors->push_back(std::move(val_descriptor));
}

void AddProblemPredicates(Puzzle::Solver* s) {
  for (int i = 0; i < 8; ++i) {
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

  for (int i = 0; i < 8; ++i) {
    if (i % 3 == 0) continue;
     
    s->AddPredicate(
        absl::StrCat("No box dupes ", i + 1),
        [i](const Puzzle::Solution& s) {
          for (int row = 0; row < 8; ++row) {
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

void AddRulePredicates(Puzzle::Solver* s) {
  // Sudoko problem specifics here.
}
