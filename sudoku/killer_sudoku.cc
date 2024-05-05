#include "sudoku/killer_sudoku.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"

ABSL_FLAG(bool, sudoku_killer_composition, false,
          "If true, adds predicates not just on the sum boxes themselves, "
          "but also add predicates for implicit maximum values from the "
          "potential input sums (e.g. the sum of 3 boxes being 7 implies "
          "no value in the cage is > 4).");

namespace sudoku {

absl::Status KillerSudoku::AddCage(const Cage& cage) {
  // kMinSums[i] = SUM(i) for i IN {1 .. i};
  constexpr int kMinSums[9] = {0, 1, 3, 6, 10, 15, 21, 28, 36};
  // kMaxSums[i] = SUM(i) for i IN {9 - i .. 9};
  constexpr int kMaxSums[9] = {0, 9, 17, 24, 30, 35, 39, 42, 44};

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
    // TODO: This isn't right! We're assuming a distinctness across boxes.
    //       This needs to have the right distinctness check before adding
    //       back.
    int max_cage_val = cage.expected_sum - kMinSums[cage.boxes.size() - 1];
    if (max_cage_val < 9) {
      for (const Box& box : cage.boxes) {
        absl::Status st = AddSpecificEntryPredicate(
            absl::StrCat("Cage max for ", box, " = ", max_cage_val),
            [box, max_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) <= max_cage_val - 1;
            },
            {box.class_id}, box.entry_id);
        if (!st.ok()) return st;
      }
    }

    int min_cage_val = cage.expected_sum - kMaxSums[cage.boxes.size() - 1];
    if (min_cage_val > 1) {
      for (const Box& box : cage.boxes) {
        absl::Status st = AddSpecificEntryPredicate(
            absl::StrCat("Cage min for ", box, " = ", min_cage_val),
            [box, min_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) >= min_cage_val - 1;
            },
            {box.class_id}, box.entry_id);
        if (!st.ok()) return st;
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
