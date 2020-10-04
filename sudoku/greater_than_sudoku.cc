#include "sudoku/greater_than_sudoku.h"

namespace sudoku {

absl::Status GreaterThanSudoku::AddComparison(const std::pair<Box, Box>& cmp) {
  if (abs(cmp.first.entry_id - cmp.second.entry_id) +
          abs(cmp.first.class_id - cmp.second.class_id) !=
      1) {
    return absl::InvalidArgumentError(
        "Comparison must be on neighboring boxes");
  }
  if (cmp.first.entry_id / 3 != cmp.second.entry_id / 3) {
    return absl::InvalidArgumentError(
        "Comparisons must be with in a box (entry)");
  }
  if (cmp.first.class_id / 3 != cmp.second.class_id / 3) {
    return absl::InvalidArgumentError(
        "Comparisons must be with in a box (class)");
  }
  if (cmp.first.entry_id == cmp.second.entry_id) {
    return AddSpecificEntryPredicate(
        absl::StrCat(cmp.first.DebugString(), " > ", cmp.second.DebugString()),
        [cmp](const puzzle::Entry& e) {
          return e.Class(cmp.first.class_id) > e.Class(cmp.second.class_id);
        },
        {cmp.first.class_id, cmp.second.class_id}, cmp.first.entry_id);
  }
  if (cmp.first.class_id != cmp.second.class_id) {
    return absl::InvalidArgumentError(
        "Comparison must be on neighboring boxes");
  }
  return AddPredicate(
      absl::StrCat(cmp.first.DebugString(), " > ", cmp.second.DebugString()),
      [cmp](const puzzle::Solution& s) {
        return s.Id(cmp.first.entry_id).Class(cmp.first.class_id) >
               s.Id(cmp.second.entry_id).Class(cmp.second.class_id);
      },
      {cmp.first.class_id});
}

absl::Status GreaterThanSudoku::InstanceSetup() {
  for (const std::pair<Box, Box>& cmp : GetComparisons()) {
    absl::Status st = AddComparison(cmp);
    if (!st.ok()) return st;
  }
  return absl::OkStatus();
}

}  // namespace sudoku
