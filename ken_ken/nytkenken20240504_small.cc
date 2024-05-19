#include "ken_ken/ken_ken.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace ken_ken {

class NYTKenKen20240504Small : public KenKen<4> {
 public:
  NYTKenKen20240504Small() = default;

  absl::StatusOr<std::string_view> GetCageKenKen() const override;

 private:
  absl::StatusOr<Board> GetSolutionBoard() const override;
};

absl::StatusOr<std::string_view> NYTKenKen20240504Small::GetCageKenKen() const {
  return R"BOARD(7+
7+
10+
6+
1+
9+

0111
0233
2243
2555)BOARD";
}

absl::StatusOr<NYTKenKen20240504Small::Board>
NYTKenKen20240504Small::GetSolutionBoard() const {
  return ToBoard(
      "3142"
      "4321"
      "2413"
      "1234");
}

REGISTER_PROBLEM(NYTKenKen20240504Small);

}  // namespace ken_ken
