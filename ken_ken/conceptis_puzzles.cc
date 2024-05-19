#include "ken_ken/ken_ken.h"

namespace ken_ken {

// This specific problem comes from
// https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
class ConceptisPuzzles : public KenKen<9> {
 public:
  ConceptisPuzzles() = default;

  absl::StatusOr<std::string_view> GetCageKenKen() const override;

 private:
  absl::StatusOr<Board> GetSolutionBoard() const override;
};

absl::StatusOr<std::string_view> ConceptisPuzzles::GetCageKenKen() const {
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

absl::StatusOr<ConceptisPuzzles::Board> ConceptisPuzzles::GetSolutionBoard() const {
  return ToBoard(
      "927351468"
      "293647581"
      "178523946"
      "619485273"
      "782196354"
      "346719825"
      "531864792"
      "465278139"
      "854932617"
  );
};

REGISTER_PROBLEM(ConceptisPuzzles);

}  // namespace ken_ken
