#include "sudoku/greater_than_sudoku.h"

namespace sudoku {

void GreaterThanSudoku::AddComparison(const std::pair<Box, Box>& cmp) {
  CHECK_EQ(abs(cmp.first.entry_id - cmp.second.entry_id) +
               abs(cmp.first.class_id - cmp.second.class_id),
           1)
      << "Comparison must be on neighboring boxes";
  CHECK_EQ(cmp.first.entry_id / 3, cmp.second.entry_id / 3)
    << "Comparisons must be with in a box (entry)";
  CHECK_EQ(cmp.first.class_id / 3, cmp.second.class_id / 3)
    << "Comparisons must be with in a box (class)";
  AddPredicate(
      absl::StrCat(cmp.first.DebugString(), " > ", cmp.second.DebugString()),
      [cmp](const puzzle::Solution& s) {
        return s.Id(cmp.first.entry_id).Class(cmp.first.class_id) >
               s.Id(cmp.second.entry_id).Class(cmp.second.class_id);
      },
      {{cmp.first.class_id, cmp.first.entry_id},
       {cmp.second.class_id, cmp.second.entry_id}});
}

void GreaterThanSudoku::InstanceSetup() {
  for (const std::pair<Box, Box>& cmp : GetComparisons()) {
    AddComparison(cmp);
  }
}

}  // namespace sudoku
