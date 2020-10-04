#include "sudoku/base.h"

#include <iostream>

#include "absl/flags/flag.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "gflags/gflags.h"

ABSL_FLAG(std::string, sudoku_problem_setup, "pairwise",
          "Sepecifies the form of the predicates passed to the puzzle "
          "solver to validate sudoku boards. Valid vaules are 'cumulative' "
          "and 'pairwise'. 'cumulative' is faster if predicate reordering "
          "is disabled, but 'pairwise' is better suited for predicate "
          "reordering and results in faster overall evaluation if "
          "reordering is enabled.");

ABSL_FLAG(bool, sudoku_setup_only, false,
          "If true, only set up predicates for valid sudoku board "
          "configuration rather than solving a specific board.");

namespace sudoku {

void Base::AddPredicatesCumulative() {
  std::vector<int> cols = {0};
  for (int i = 1; i < 9; ++i) {
    cols.push_back(i);
    AddAllEntryPredicate(
        absl::StrCat("No row dupes ", i + 1),
        [i](const puzzle::Entry& e) {
          for (int j = 0; j < i; ++j) {
            if (e.Class(i) == e.Class(j)) return false;
          }
          return true;
        },
        cols);
  }

  for (int i = 0; i < 9; ++i) {
    if (i % 3 == 0) {
      cols = {i};
      continue;
    }

    cols.push_back(i);
    AddPredicate(
        absl::StrCat("No box dupes ", i + 1),
        [i](const puzzle::Solution& s) {
          for (int row = 0; row < 9; ++row) {
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
        cols);
  }
}

void Base::AddPredicatesPairwise() {
  for (int i = 0; i < 9; ++i) {
    for (int j = i + 1; j < 9; ++j) {
      AddAllEntryPredicate(
          absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
          [i, j](const puzzle::Entry& e) { return e.Class(i) != e.Class(j); },
          {i, j});
    }
  }

  for (int box = 0; box < 9; ++box) {
    const int box_base_x = 3 * (box / 3);
    const int box_base_y = 3 * (box % 3);

    for (int i = 0; i < 9; ++i) {
      const int box_i_x = box_base_x + (i / 3);
      const int box_i_y = box_base_y + (i % 3);

      for (int j = i + 1; j < 9; ++j) {
        const int box_j_x = box_base_x + (j / 3);
        const int box_j_y = box_base_y + (j % 3);

        CHECK(!(box_i_x == box_j_x && box_i_y == box_j_y));

        // Handled by class permutation.
        if (box_i_x == box_j_x) continue;

        // Handled by row predicate.
        if (box_i_y == box_j_y) continue;

        AddPredicate(
            absl::StrCat("No box dupes "
                         "(",
                         box_i_x + 1, ",", box_i_y + 1,
                         ") vs "
                         "(",
                         box_j_x + 1, ",", box_j_y + 1, ")"),
            [box_i_x, box_i_y, box_j_x, box_j_y](const puzzle::Solution& s) {
              return s.Id(box_i_x).Class(box_i_y) !=
                     s.Id(box_j_x).Class(box_j_y);
            },
            {{box_i_y, box_i_x}, {box_j_y, box_j_x}});
      }
    }
  }
}

void Base::AddValuePredicate(int row, int col, int value) {
  AddSpecificEntryPredicate(
      absl::StrCat("(", row + 1, ",", col + 1, ") = ", value),
      [col, value](const puzzle::Entry& e) { return e.Class(col) == value; },
      {col}, row);
}

void Base::AddBoardPredicates(const Board& board) {
  CHECK_EQ(board.size(), 9);
  for (size_t row = 0; row < board.size(); ++row) {
    CHECK_EQ(board[row].size(), 9);
    for (size_t col = 0; col < board[row].size(); ++col) {
      if (board[row][col] > 0) {
        AddValuePredicate(row, col, board[row][col] - 1);
      }
    }
  }
}

absl::StatusOr<puzzle::Solution> Base::GetSolution() const {
  Board board = GetSolutionBoard();
  std::vector<puzzle::Entry> entries;
  if (board.size() != 9) {
    return absl::InvalidArgumentError("Board must have 9 rows");
  }
  for (size_t row = 0; row < board.size(); ++row) {
    if (board[row].size() != 9) {
      return absl::InvalidArgumentError(
          "Board must have 9 columns in each row");
    }
    for (size_t col = 0; col < board[row].size(); ++col) {
      // Translate to 0-indexed solution space.
      --board[row][col];
    }
    entries.emplace_back(row, board[row], entry_descriptor());
  }
  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

// static
Base::Board Base::ParseBoard(const absl::string_view board) {
  Board ret;
  std::vector<std::string> rows = absl::StrSplit(board, "\n");
  CHECK_EQ(rows.size(), /*data=*/9 + /*spacer=*/2);
  for (const absl::string_view row : rows) {
    if (row == "- - - + - - - + - - -") continue;

    std::vector<std::string> cols = absl::StrSplit(row, " ");
    CHECK_EQ(cols.size(), /*data=*/9 + /*spacer=*/2);
    std::vector<int> cur_cols;
    for (const absl::string_view col : cols) {
      if (col == "|") continue;
      int val = -1;
      if (col != "?") {
        CHECK(absl::SimpleAtoi(col, &val)) << "Not a number: " << col;
      }
      cur_cols.push_back(val);
    }
    ret.push_back(cur_cols);
  }
  return ret;
}

void Base::InstanceSetup() { AddBoardPredicates(GetInstanceBoard()); }

void Base::Setup() {
  // Descriptors are built so solution.DebugString(), kinda, sorta looks like
  // a sudoku board.
  puzzle::StringDescriptor* id_descriptor =
      AddDescriptor(new puzzle::StringDescriptor());
  for (int i = 0; i < 9; i += 3) {
    // Every third row (starting with the first) gets a horizontal line.
    id_descriptor->SetDescription(i, "+----------+----------+----------+\n");
    id_descriptor->SetDescription(i + 1, "");
    id_descriptor->SetDescription(i + 2, "");
  }
  SetIdentifiers(id_descriptor);

  // Most records we want to describe the 0-indexed value with the 1-indexed
  // numeric value...
  puzzle::StringDescriptor* mid_val_descriptor =
      AddDescriptor(new puzzle::StringDescriptor());
  for (int i = 0; i < 9; i++) {
    mid_val_descriptor->SetDescription(i, absl::StrCat(i + 1));
  }
  // ... but every third one we also add ' :' to add a visual virtical line
  // in output printing.
  puzzle::StringDescriptor* col_val_descriptor =
      AddDescriptor(new puzzle::StringDescriptor());
  for (int i = 0; i < 9; i++) {
    col_val_descriptor->SetDescription(i, absl::StrCat(i + 1, " :"));
  }
  for (int i = 0; i < 9; ++i) {
    AddClass(i, "", (i % 3) == 2 ? col_val_descriptor : mid_val_descriptor);
  }

  if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "cumulative") {
    AddPredicatesCumulative();
  } else if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "pairwise") {
    AddPredicatesPairwise();
  } else {
    LOG(FATAL) << "Unrecognized option for sudoku_problem_setup '"
               << absl::GetFlag(FLAGS_sudoku_problem_setup)
               << "'; valid values are "
               << "'cumulative' and 'pairwise'.";
  }

  if (absl::GetFlag(FLAGS_sudoku_setup_only)) {
    return;
  }

  InstanceSetup();
}

}  // namespace sudoku
