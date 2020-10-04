#include "sudoku/killer_sudoku.h"

#include "absl/strings/str_cat.h"

namespace sudoku {

absl::Status KillerSudoku::AddCage(const Cage& cage) {
  if (cage.boxes.empty()) {
    return absl::InvalidArgumentError("cage cannot be empty");
  }
  absl::flat_hash_map<int, int> class_to_entry;
  for (const Box& box : cage.boxes) {
    if (box_used_.contains(box)) {
      return absl::InvalidArgumentError(
          absl::StrCat("Duplicate entry: ", box.DebugString()));
    }
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
  return AddPredicate(
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

absl::Status KillerSudoku::InstanceSetup() {
  for (const Cage& cage : GetCages()) {
    absl::Status st = AddCage(cage);
    if (!st.ok()) return st;
  }

  for (int entry_id = 0; entry_id < 9; ++entry_id) {
    for (int class_id = 0; class_id < 9; ++class_id) {
      Box b = {entry_id, class_id};
      if (!box_used_.contains(b)) {
        return absl::InvalidArgumentError(
            absl::StrCat("Missing entry: ", b.DebugString()));
      }
    }
  }

  return absl::OkStatus();
}

}  // namespace sudoku
