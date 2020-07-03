#include "sudoku/killer_sudoku.h"

namespace sudoku {

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
      // If a predicate uses multiple entries for a class, there isn't a skip
      // model that captures that.
      it->second = puzzle::Entry::kBadId;
    }
  }
  AddPredicate(
      absl::StrCat("Sum around ", cage.boxes[0].DebugString(), " = ",
                   cage.expected_sum),
      [cage](const puzzle::Solution& s) {
        // Solution values are 0-indexed, rather than 1-indexed like the sum.
        int sum = cage.boxes.size();
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
      Box b = {entry_id, class_id};
      CHECK(box_used_.contains(b)) << b;
    }
  }
}

}  // namespace sudoku
