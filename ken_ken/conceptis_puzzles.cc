#include "ken_ken/board.h"

namespace ken_ken {

// This specific problem comes from
// https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
class ConceptisPuzzles : public Board<9> {
 public:
  ConceptisPuzzles() = default;

  absl::StatusOr<std::string_view> GetCageBoard() const override;

 private:
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

absl::StatusOr<std::string_view> ConceptisPuzzles::GetCageBoard() const {
  return R"BOARD(21+
60*
25+
6-
11+
4-
13+
8+
8+
2+
9+
11+
24+
13+
25+
3-
17+
2+
3+
1-
13+
1-
11+
26+
1+
16*
6+
12+
7+
25+
1-
378*
3*

000111222
033415662
078495AB2
C7DDEFFBG
CCHEEEIGG
CJKKELLMG
NJOPQRSMT
NUUPVRWWT
NNNVVVTTT)BOARD";
}

absl::StatusOr<puzzle::Solution> ConceptisPuzzles::GetSolution() const {
  const std::vector<std::vector<int>> res = {
      {9, 2, 7, 3, 5, 1, 4, 6, 8}, {2, 9, 3, 6, 4, 7, 5, 8, 1},
      {1, 7, 8, 5, 2, 3, 9, 4, 6}, {6, 1, 9, 4, 8, 5, 2, 7, 3},
      {7, 8, 2, 1, 9, 6, 3, 5, 4}, {3, 4, 6, 7, 1, 9, 8, 2, 5},
      {5, 3, 1, 8, 6, 4, 7, 9, 2}, {4, 6, 5, 2, 7, 8, 1, 3, 9},
      {8, 5, 4, 9, 3, 2, 6, 1, 7},
  };
  std::vector<puzzle::Entry> entries;
  for (int i = 0; i < 9; ++i) {
    std::vector<int> vals = res[i];
    // Fencepost.
    for (int& v : vals) --v;
    entries.emplace_back(i, vals, entry_descriptor());
  }

  return puzzle::Solution(entry_descriptor(), &entries).Clone();
};

REGISTER_PROBLEM(ConceptisPuzzles);

}  // namespace ken_ken
