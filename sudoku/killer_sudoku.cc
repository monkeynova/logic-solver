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

void KillerSudoku::AddSum(int expected_sum, std::vector<Box> boxes) {
  CHECK(!boxes.empty());
  absl::flat_hash_map<int, int> class_to_entry;
  for (const Box& box : boxes) {
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
      absl::StrCat("Sum around ", boxes[0].DebugString(), " = ", expected_sum),
      [expected_sum, boxes](const puzzle::Solution& s) {
        int sum = 0;
        for (const Box& box : boxes) {
          sum += s.Id(box.entry_id).Class(box.class_id);
        }
        return sum == expected_sum;
      },
      std::move(class_to_entry));
}

void KillerSudoku::InstanceSetup() {
  // This specific problem comes from
  // https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
  AddSum(15, {{0, 0}, {0, 1}, {1, 0}});
  AddSum(19, {{0, 2}, {0, 3}, {1, 2}});
  AddSum(16, {{0, 4}, {0, 5}, {1, 4}});
  AddSum(8, {{0, 6}, {0, 7}, {1, 7}});
  AddSum(19, {{0, 8}, {1, 8}, {2, 8}});
  AddSum(15, {{1, 1}, {2, 1}, {3, 1}});
  AddSum(4, {{1, 3}, {2, 3}});
  AddSum(18, {{1, 5}, {1, 6}, {2, 6}});
  AddSum(7, {{2, 0}, {3, 0}});
  AddSum(16, {{2, 2}, {3, 2}, {3, 3}});
  AddSum(16, {{2, 4}, {2, 5}, {3, 4}});
  AddSum(12, {{2, 7}, {3, 7}, {4, 7}});
  AddSum(17, {{3, 5}, {3, 6}, {4, 5}});
  AddSum(17, {{3, 8}, {4, 8}, {5, 8}});
  AddSum(17, {{4, 0}, {5, 0}, {5, 1}});
  AddSum(10, {{4, 1}, {4, 2}});
  AddSum(20, {{4, 3}, {4, 4}, {5, 3}});
  AddSum(7, {{4, 6}, {5, 6}});
  AddSum(6, {{5, 2}, {6, 2}});
  AddSum(9, {{5, 4}, {6, 3}, {6, 4}});
  AddSum(7, {{5, 5}, {6, 5}});
  AddSum(26, {{5, 7}, {6, 7}, {6, 8}, {7, 7}, {7, 8}});
  AddSum(23, {{6, 0}, {6, 1}, {7, 0}, {7, 1}});
  AddSum(12, {{6, 6}, {7, 6}});
  AddSum(26, {{7, 2}, {7, 3}, {7, 4}, {7, 5}, {8, 4}});
  AddSum(21, {{8, 0}, {8, 1}, {8, 2}, {8, 3}});
  AddSum(12, {{8, 5}, {8, 6}});
  AddSum(10, {{8, 7}, {8, 8}});

  for (int entry_id = 0; entry_id < 9; ++entry_id) {
    for (int class_id = 0; class_id < 9; ++class_id) {
      Box b = {.entry_id = entry_id, .class_id = class_id};
      CHECK(box_used_.contains(b)) << b;
    }
  }
}

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::KillerSudoku);
