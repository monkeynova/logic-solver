#include "sudoku/killer_sudoku.h"

namespace sudoku {

// This specific problem comes from
// https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
class ConceptisPuzzles : public KillerSudoku {
 public:
  std::vector<KillerSudoku::Cage> GetCages() const override {
    return std::vector<Cage>{
        {.expected_sum = 15, .boxes = {{0, 0}, {0, 1}, {1, 0}}},
        {.expected_sum = 19, .boxes = {{0, 2}, {0, 3}, {1, 2}}},
        {.expected_sum = 16, .boxes = {{0, 4}, {0, 5}, {1, 4}}},
        {.expected_sum = 8, .boxes = {{0, 6}, {0, 7}, {1, 7}}},
        {.expected_sum = 19, .boxes = {{0, 8}, {1, 8}, {2, 8}}},
        {.expected_sum = 15, .boxes = {{1, 1}, {2, 1}, {3, 1}}},
        {.expected_sum = 4, .boxes = {{1, 3}, {2, 3}}},
        {.expected_sum = 18, .boxes = {{1, 5}, {1, 6}, {2, 6}}},
        {.expected_sum = 7, .boxes = {{2, 0}, {3, 0}}},
        {.expected_sum = 16, .boxes = {{2, 2}, {3, 2}, {3, 3}}},
        {.expected_sum = 16, .boxes = {{2, 4}, {2, 5}, {3, 4}}},
        {.expected_sum = 12, .boxes = {{2, 7}, {3, 7}, {4, 7}}},
        {.expected_sum = 17, .boxes = {{3, 5}, {3, 6}, {4, 5}}},
        {.expected_sum = 17, .boxes = {{3, 8}, {4, 8}, {5, 8}}},
        {.expected_sum = 17, .boxes = {{4, 0}, {5, 0}, {5, 1}}},
        {.expected_sum = 10, .boxes = {{4, 1}, {4, 2}}},
        {.expected_sum = 20, .boxes = {{4, 3}, {4, 4}, {5, 3}}},
        {.expected_sum = 7, .boxes = {{4, 6}, {5, 6}}},
        {.expected_sum = 6, .boxes = {{5, 2}, {6, 2}}},
        {.expected_sum = 9, .boxes = {{5, 4}, {6, 3}, {6, 4}}},
        {.expected_sum = 7, .boxes = {{5, 5}, {6, 5}}},
        {.expected_sum = 26, .boxes = {{5, 7}, {6, 7}, {6, 8}, {7, 7}, {7, 8}}},
        {.expected_sum = 23, .boxes = {{6, 0}, {6, 1}, {7, 0}, {7, 1}}},
        {.expected_sum = 12, .boxes = {{6, 6}, {7, 6}}},
        {.expected_sum = 26, .boxes = {{7, 2}, {7, 3}, {7, 4}, {7, 5}, {8, 4}}},
        {.expected_sum = 21, .boxes = {{8, 0}, {8, 1}, {8, 2}, {8, 3}}},
        {.expected_sum = 12, .boxes = {{8, 5}, {8, 6}}},
        {.expected_sum = 10, .boxes = {{8, 7}, {8, 8}}}};
  }

  Board GetSolutionBoard() const override {
    return {{8, 6, 3, 9, 7, 5, 2, 1, 4}, {1, 2, 7, 3, 4, 6, 9, 5, 8},
            {4, 5, 9, 1, 8, 2, 3, 6, 7}, {3, 8, 5, 2, 6, 1, 7, 4, 9},
            {7, 4, 6, 8, 5, 9, 1, 2, 3}, {9, 1, 2, 7, 3, 4, 6, 8, 5},
            {2, 9, 4, 5, 1, 3, 8, 7, 6}, {5, 7, 1, 6, 9, 8, 4, 3, 2},
            {6, 3, 8, 4, 2, 7, 5, 9, 1}};
  }
};

}  // namespace sudoku

REGISTER_PROBLEM(sudoku::ConceptisPuzzles);
