#include "sudoku/killer_sudoku.h"

namespace sudoku {

// This specific problem comes from
// https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
class ConceptisPuzzles : public KillerSudoku {
 public:
  std::vector<KillerSudoku::Cage> GetCages() const override {
    return std::vector<Cage>{{15, {{0, 0}, {0, 1}, {1, 0}}},
                             {19, {{0, 2}, {0, 3}, {1, 2}}},
                             {16, {{0, 4}, {0, 5}, {1, 4}}},
                             {8, {{0, 6}, {0, 7}, {1, 7}}},
                             {19, {{0, 8}, {1, 8}, {2, 8}}},
                             {15, {{1, 1}, {2, 1}, {3, 1}}},
                             {4, {{1, 3}, {2, 3}}},
                             {18, {{1, 5}, {1, 6}, {2, 6}}},
                             {7, {{2, 0}, {3, 0}}},
                             {16, {{2, 2}, {3, 2}, {3, 3}}},
                             {16, {{2, 4}, {2, 5}, {3, 4}}},
                             {12, {{2, 7}, {3, 7}, {4, 7}}},
                             {17, {{3, 5}, {3, 6}, {4, 5}}},
                             {17, {{3, 8}, {4, 8}, {5, 8}}},
                             {17, {{4, 0}, {5, 0}, {5, 1}}},
                             {10, {{4, 1}, {4, 2}}},
                             {20, {{4, 3}, {4, 4}, {5, 3}}},
                             {7, {{4, 6}, {5, 6}}},
                             {6, {{5, 2}, {6, 2}}},
                             {9, {{5, 4}, {6, 3}, {6, 4}}},
                             {7, {{5, 5}, {6, 5}}},
                             {26, {{5, 7}, {6, 7}, {6, 8}, {7, 7}, {7, 8}}},
                             {23, {{6, 0}, {6, 1}, {7, 0}, {7, 1}}},
                             {12, {{6, 6}, {7, 6}}},
                             {26, {{7, 2}, {7, 3}, {7, 4}, {7, 5}, {8, 4}}},
                             {21, {{8, 0}, {8, 1}, {8, 2}, {8, 3}}},
                             {12, {{8, 5}, {8, 6}}},
                             {10, {{8, 7}, {8, 8}}}};
  }

  absl::StatusOr<Board> GetSolutionBoard() const override {
    return Board{std::array<int, 9>{8, 6, 3, 9, 7, 5, 2, 1, 4},
                 {1, 2, 7, 3, 4, 6, 9, 5, 8},
                 {4, 5, 9, 1, 8, 2, 3, 6, 7},
                 {3, 8, 5, 2, 6, 1, 7, 4, 9},
                 {7, 4, 6, 8, 5, 9, 1, 2, 3},
                 {9, 1, 2, 7, 3, 4, 6, 8, 5},
                 {2, 9, 4, 5, 1, 3, 8, 7, 6},
                 {5, 7, 1, 6, 9, 8, 4, 3, 2},
                 {6, 3, 8, 4, 2, 7, 5, 9, 1}};
  }
};

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::ConceptisPuzzles);
