#include "sudoku/killer_sudoku.h"

namespace sudoku {

Base::Board KillerSudoku::GetInstanceBoard() const { return Board(); }

Base::Board KillerSudoku::GetSolutionBoard() const {
  return {{8, 6, 3, 9, 7, 5, 2, 1, 4}, {1, 2, 7, 3, 4, 6, 9, 5, 8},
          {4, 5, 9, 1, 8, 2, 3, 6, 7}, {3, 8, 5, 2, 6, 1, 7, 4, 9},
          {7, 4, 6, 8, 5, 9, 1, 2, 3}, {9, 1, 2, 7, 3, 4, 6, 8, 5},
          {2, 9, 4, 5, 1, 3, 8, 7, 6}, {5, 7, 1, 6, 9, 8, 4, 3, 2},
          {6, 3, 8, 4, 2, 7, 5, 9, 1}};
}

void KillerSudoku::AddCage(const Cage& cage) {
  CHECK(!cage.boxes.empty());
  absl::flat_hash_map<int, int> class_to_entry;
  for (const Box& box : cage.boxes) {
    CHECK(!box_used_.contains(box)) << "Duplicate entry: " << box;
    box_used_.insert(box);
    auto it = class_to_entry.find(box.class_id);
    if (it == class_to_entry.end()) {
      class_to_entry.emplace(box.class_id, box.entry_id);
    } else {
      it->second = std::max(it->second, box.entry_id);
    }
  }
  AddPredicate(
      absl::StrCat("Sum around ", cage.boxes[0].DebugString(), " = ",
                   cage.expected_sum),
      [cage](const puzzle::Solution& s) {
        int sum = 0;
        for (const Box& box : cage.boxes) {
          sum += s.Id(box.entry_id).Class(box.class_id);
        }
        return sum == cage.expected_sum;
      },
      std::move(class_to_entry));
}

void KillerSudoku::InstanceSetup() {
  for (const Cage& cage : GetCages()) {
    AddCage(cage);
  }

  for (int entry_id = 0; entry_id < 9; ++entry_id) {
    for (int class_id = 0; class_id < 9; ++class_id) {
      Box b = {.entry_id = entry_id, .class_id = class_id};
      CHECK(box_used_.contains(b)) << b;
    }
  }
}

}  // namespace sudoku
