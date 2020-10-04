#ifndef SUDOKU_BASE_H
#define SUDOKU_BASE_H

#include <string>
#include <vector>

#include "puzzle/problem.h"

namespace sudoku {

/*
Logic solver repurposed for sudoku
 */
class Base : public ::puzzle::Problem {
 public:
  using Board = std::vector<std::vector<int>>;

  static Board EmptyBoard() { return Board(9, std::vector<int>(9, 0)); }

  struct Box {
    int entry_id;
    int class_id;

    std::string DebugString() const {
      return absl::StrCat("(", entry_id, ", ", class_id, ")");
    }

    template <typename H>
    friend H AbslHashValue(H h, const Box& box) {
      return H::combine(std::move(h), box.entry_id, box.class_id);
    }

    bool operator==(const Box& other) const {
      return entry_id == other.entry_id && class_id == other.class_id;
    }
  };

  static absl::StatusOr<Board> ParseBoard(const absl::string_view board);

  virtual absl::StatusOr<Board> GetInstanceBoard() const = 0;
  virtual absl::StatusOr<Board> GetSolutionBoard() const = 0;

 protected:
  virtual absl::Status InstanceSetup();

 private:
  // ::puzzle::Problem methods. Final to prevent missing parts.
  absl::Status Setup() final;
  absl::StatusOr<::puzzle::Solution> GetSolution() const final;

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  absl::Status AddValuePredicate(int row, int col, int value);
  absl::Status AddBoardPredicates(const Board& board);
  absl::Status AddPredicates();
  absl::Status AddPredicatesCumulative();
  absl::Status AddPredicatesPairwise();
};

inline std::ostream& operator<<(std::ostream& out, const Base::Box& box) {
  return out << box.DebugString();
}

}  // namespace sudoku

#endif  // SUDOKU_BASE_H
