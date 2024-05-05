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
  return absl::UnimplementedError("Unsolved");
};

REGISTER_PROBLEM(ConceptisPuzzles);

}  // namespace ken_ken
