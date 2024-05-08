#include "sudoku/killer_sudoku.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"

ABSL_FLAG(bool, sudoku_killer_composition, true,
          "If true, adds predicates not just on the sum boxes themselves, "
          "but also add predicates for implicit maximum values from the "
          "potential input sums (e.g. the sum of 3 boxes being 7 implies "
          "no value in the cage is > 4).");

namespace sudoku {

absl::Status KillerSudoku::AddCage(const Cage& cage) {
  if (cage.boxes.empty()) {
    return absl::InvalidArgumentError("cage cannot be empty");
  }
  absl::flat_hash_map<int, int> class_to_entry;
  for (const Box& box : cage.boxes) {
    if (box_used_.contains(box)) {
      return absl::InvalidArgumentError(absl::StrCat("Duplicate entry: ", box));
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

  if (absl::GetFlag(FLAGS_sudoku_killer_composition)) {
    std::vector<int> count_by_entry(9, 0);
    std::vector<int> count_by_class(9, 0);
    for (const Box& box : cage.boxes) {
      ++count_by_entry[box.entry_id];
      ++count_by_class[box.class_id];
    }
    int min_by_entry = 0;
    int min_by_class = 0;
    int max_by_entry = 0;
    int max_by_class = 0;
    for (int i = 0; i < 9; ++i) {
      min_by_entry += count_by_entry[i] * (count_by_entry[i] + 1) / 2;
      min_by_class += count_by_class[i] * (count_by_class[i] + 1) / 2;
      max_by_entry += count_by_entry[i] * (count_by_entry[i] + 1) / 2 +
                      (9 - count_by_entry[i]) * count_by_entry[i];
      max_by_class += count_by_class[i] * (count_by_class[i] + 1) / 2 +
                      (9 - count_by_class[i]) * count_by_class[i];
    }
    int min_cage = std::min(min_by_entry, min_by_class);
    int max_cage = std::min(max_by_entry, max_by_class);

    for (const Box& box : cage.boxes) {
      int biggest_remove =
          std::max(count_by_entry[box.entry_id], count_by_class[box.class_id]);
      int max_cage_val = cage.expected_sum - (min_cage - biggest_remove);
      if (max_cage_val < 9) {
        RETURN_IF_ERROR(AddSpecificEntryPredicate(
            absl::StrCat("Cage max for ", box, " = ", max_cage_val),
            [box, max_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) <= max_cage_val - 1;
            },
            {box.class_id}, box.entry_id));
      }

      int smallest_remove = 9 + 1 - biggest_remove;
      int min_cage_val = cage.expected_sum - (max_cage - smallest_remove);
      if (min_cage_val > 1) {
        RETURN_IF_ERROR(AddSpecificEntryPredicate(
            absl::StrCat("Cage min for ", box, " = ", min_cage_val),
            [box, min_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) >= min_cage_val - 1;
            },
            {box.class_id}, box.entry_id));
      }
    }

    // TODO(@monkeynova): There are likely other restrictions possible. For
    // example a SUM of 4 precludes the value 2 from either given the sudoku
    // constraints.
  }

  return AddPredicate(
      absl::StrCat("Sum around ", cage.boxes[0], " = ", cage.expected_sum),
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
        return absl::InvalidArgumentError(absl::StrCat("Missing entry: ", b));
      }
    }
  }

  return absl::OkStatus();
}

}  // namespace sudoku
