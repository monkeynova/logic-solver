#include "sudoku/sudoku.h"

#include <iostream>

#include "absl/flags/flag.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"

extern absl::Flag<std::string> FLAGS_sudoku_problem_setup;

ABSL_FLAG(bool, sudoku_setup_only, false,
          "If true, only set up predicates for valid sudoku board "
          "configuration rather than solving a specific board.");

// TODO(@monkeynova): Figure out how to do this composition in the puzzle
// solver itself.
ABSL_FLAG(bool, sudoku_setup_composed_value_predicates, true,
          "If true, value predicates (those of the form \"this square is "
          "exactly this value\") are composed with row and box predicates "
          "to created implicit != predicates on the individual squares). "
          "This currently (2021.01.21) results in a significant performance "
          "improvement over not using them.");

namespace sudoku {

// TODO: Move this to ken_ken::Grid?
puzzle::EntryDescriptor Sudoku::MakeEntryDescriptor() {
  // Descriptors are built so absl::StrCat(solution), kinda, sorta looks like
  // a sudoku board.
  std::vector<std::string> id_names(kWidth);
  for (int i = 0; i < kWidth; i += kSubHeight) {
    // Every third row (starting with the first) gets a horizontal line.
    id_names[i] = ("+----------+----------+----------+\n");
    for (int j = 1; j < kSubHeight; ++j) {
      id_names[i + j] = ("");
    }
  }

  std::vector<std::unique_ptr<const puzzle::Descriptor>> class_descriptors;
  for (int i = 0; i < kWidth; ++i) {
    std::vector<std::string> class_names(kWidth);
    if (i % kSubHeight != kSubHeight - 1) {
      // Most records we want to describe the 0-indexed value with the 1-indexed
      // numeric value...
      for (int j = 0; j < kWidth; ++j) {
        class_names[j] = absl::StrCat(j + 1);
      }
    } else {
      // ... but every third one we also add ' :' to add a visual virtical line
      // in output printing.
      for (int j = 0; j < kWidth; ++j) {
        class_names[j] = absl::StrCat(j + 1, " :");
      }
    }
    class_descriptors.push_back(
        absl::make_unique<puzzle::StringDescriptor>(std::move(class_names)));
  }

  return puzzle::EntryDescriptor(
      absl::make_unique<puzzle::StringDescriptor>(id_names),
      /*class_descriptor=*/
      absl::make_unique<puzzle::StringDescriptor>(
          std::vector<std::string>(kWidth, "")),
      std::move(class_descriptors));
}

absl::Status Sudoku::AddPredicatesCumulative() {
  std::vector<int> cols;
  for (int i = 0; i < kWidth; ++i) {
    if (i % kSubHeight == 0) {
      cols = {i};
      continue;
    }

    cols.push_back(i);
    absl::Status st = AddPredicate(
        absl::StrCat("No box dupes ", i + 1),
        [i](const puzzle::Solution& s) {
          for (int row = 0; row < kWidth; ++row) {
            int to_match = s.Id(row).Class(i);

            int box_base = row - (row % kSubHeight);
            for (int col = i - (i % kSubHeight); col < i; ++col) {
              for (int box_row = 0; box_row < kSubHeight; ++box_row) {
                if (s.Id(box_base + box_row).Class(col) == to_match)
                  return false;
              }
            }
          }
          return true;
        },
        cols);
    if (!st.ok()) return st;
  }
  return absl::OkStatus();
}

absl::Status Sudoku::AddPredicatesPairwise() {
  for (int box = 0; box < kWidth; ++box) {
    const int box_base_x = kSubHeight * (box / kSubHeight);
    const int box_base_y = kSubHeight * (box % kSubHeight);

    for (int i = 0; i < kWidth; ++i) {
      const int box_i_x = box_base_x + (i / kSubHeight);
      const int box_i_y = box_base_y + (i % kSubHeight);

      for (int j = i + 1; j < kWidth; ++j) {
        const int box_j_x = box_base_x + (j / kSubHeight);
        const int box_j_y = box_base_y + (j % kSubHeight);

        if (box_i_x == box_j_x && box_i_y == box_j_y) {
          return absl::InternalError("box iteration wonky");
        }

        // Handled by class permutation.
        if (box_i_x == box_j_x) continue;

        // Handled by row predicate.
        if (box_i_y == box_j_y) continue;

        absl::Status st = AddPredicate(
            absl::StrCat("No box dupes (", box_i_x + 1, ",", box_i_y + 1,
                         ") vs (", box_j_x + 1, ",", box_j_y + 1, ")"),
            [box_i_x, box_i_y, box_j_x, box_j_y](const puzzle::Solution& s) {
              return s.Id(box_i_x).Class(box_i_y) !=
                     s.Id(box_j_x).Class(box_j_y);
            },
            {{box_i_y, box_i_x}, {box_j_y, box_j_x}});
        if (!st.ok()) return st;
      }
    }
  }
  return absl::OkStatus();
}

absl::Status Sudoku::AddComposedValuePredicates(int row, int col, int value) {
  for (int i = 0; i < kWidth; ++i) {
    if (i == col) continue;
    absl::Status st = AddSpecificEntryPredicate(
        absl::StrCat("(", row + 1, ",", col + 1, ")=", value, " AND ",
                     "No row dupes(", i + 1, ")"),
        [i, value](const puzzle::Entry& e) { return e.Class(i) != value; }, {i},
        row);
    if (!st.ok()) return st;
  }

  int base_box_x = kSubHeight * (row / kSubHeight);
  int base_box_y = kSubHeight * (col / kSubHeight);
  for (int i = 0; i < kWidth; ++i) {
    int test_box_x = (i / kSubHeight) + base_box_x;
    int test_box_y = (i % kSubHeight) + base_box_y;
    if (test_box_x == row && test_box_y == col) {
      continue;
    }
    absl::Status st = AddPredicate(
        absl::StrCat("(", row + 1, ",", col + 1, ")=", value, " AND ",
                     "No box dupes "
                     "(",
                     test_box_x + 1, ",", test_box_y + 1, ")"),
        [test_box_x, test_box_y, value](const puzzle::Solution& s) {
          return s.Id(test_box_x).Class(test_box_y) != value;
        },
        absl::flat_hash_map<int, int>{{test_box_y, test_box_x}});
    if (!st.ok()) return st;
  }

  return absl::OkStatus();
}

absl::Status Sudoku::AddValuePredicate(int row, int col, int value) {
  absl::Status st = AddSpecificEntryPredicate(
      absl::StrCat("(", row + 1, ",", col + 1, ") = ", value),
      [col, value](const puzzle::Entry& e) { return e.Class(col) == value; },
      {col}, row);
  if (!st.ok()) return st;

  if (absl::GetFlag(FLAGS_sudoku_setup_composed_value_predicates)) {
    absl::Status st = AddComposedValuePredicates(row, col, value);
    if (!st.ok()) return st;
  }

  return absl::OkStatus();
}

absl::Status Sudoku::AddBoardPredicates(const Board& board) {
  if (board.size() != kWidth) {
    return absl::InvalidArgumentError(absl::StrCat("Board must have ", kWidth, " rows"));
  }
  for (size_t row = 0; row < board.size(); ++row) {
    if (board[row].size() != kWidth) {
      return absl::InvalidArgumentError(absl::StrCat("Each board row must have ", kWidth, " columns"));
    }
    for (size_t col = 0; col < board[row].size(); ++col) {
      if (board[row][col] > 0) {
        absl::Status st = AddValuePredicate(row, col, board[row][col] - 1);
        if (!st.ok()) return st;
      }
    }
  }
  return absl::OkStatus();
}

absl::StatusOr<puzzle::Solution> Sudoku::GetSolution() const {
  absl::StatusOr<Board> board = GetSolutionBoard();
  if (!board.ok()) return board.status();

  std::vector<puzzle::Entry> entries;
  for (size_t row = 0; row < board->size(); ++row) {
    std::vector<int> entry_vals(kWidth, 0);
    for (size_t col = 0; col < (*board)[row].size(); ++col) {
      // Translate to 0-indexed solution space.
      entry_vals[col] = (*board)[row][col] - 1;
    }
    entries.emplace_back(row, entry_vals, entry_descriptor());
  }
  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

// static
absl::StatusOr<Sudoku::Board> Sudoku::ParseBoard(
    const absl::string_view board) {
  Board ret;
  std::vector<std::string> rows = absl::StrSplit(board, "\n");
  if (rows.size() != /*data=*/kWidth + /*spacer=*/2) {
    return absl::InvalidArgumentError("# of rows isn't 11");
  }
  int row_idx = 0;
  for (const absl::string_view row : rows) {
    if (row == "- - - + - - - + - - -") continue;

    std::vector<std::string> cols = absl::StrSplit(row, " ");
    if (cols.size() != /*data=*/kWidth + /*spacer=*/2) {
      return absl::InvalidArgumentError("Length of row isn't 11");
    }
    int col_idx = 0;
    for (const absl::string_view col : cols) {
      if (col == "|") continue;
      int val = -1;
      if (col != "?") {
        if (!absl::SimpleAtoi(col, &val)) {
          return absl::InvalidArgumentError(
              absl::StrCat("Not a number: ", col));
        }
      }
      ret[row_idx][col_idx] = val;
      ++col_idx;
    }
    ++row_idx;
  }
  return ret;
}

absl::Status Sudoku::InstanceSetup() {
  absl::StatusOr<Board> instance = GetInstanceBoard();
  if (!instance.ok()) return instance.status();
  return AddBoardPredicates(*instance);
}

absl::Status Sudoku::AddGridPredicates() {
  if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "cumulative") {
    if (absl::Status st = AddPredicatesCumulative(); !st.ok()) return st;
  } else if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "pairwise") {
    if (absl::Status st = AddPredicatesPairwise(); !st.ok()) return st;
  } else {
    return absl::InternalError(
        absl::StrCat("Unrecognized option for sudoku_problem_setup '",
                     absl::GetFlag(FLAGS_sudoku_problem_setup),
                     "'; valid values are "
                     "'cumulative' and 'pairwise'."));
  }

  if (!absl::GetFlag(FLAGS_sudoku_setup_only)) {
    absl::Status st = InstanceSetup();
    if (!st.ok()) return st;
  }

  return absl::OkStatus();
}

}  // namespace sudoku
