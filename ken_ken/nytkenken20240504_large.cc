#include "ken_ken/ken_ken.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace ken_ken {

class NYTKenKen20240504Large : public KenKen<6> {
 public:
  NYTKenKen20240504Large() = default;

  absl::StatusOr<std::string_view> GetCageKenKen() const override;

 private:
  absl::StatusOr<Board> GetSolutionBoard() const override;
};

absl::StatusOr<std::string_view> NYTKenKen20240504Large::GetCageKenKen() const {
  return R"BOARD(8+
10*
12+
30*
1-
2-
6+
2/
18+
180*
2-
2/
10+
1+

011223
044233
556663
7789AA
B889CC
B8D99C)BOARD";
}

absl::StatusOr<NYTKenKen20240504Large::Board> NYTKenKen20240504Large::GetSolutionBoard() const {
  return ToBoard("352641" "534216" "643125" "125364" "216453" "461532");
}

REGISTER_PROBLEM(NYTKenKen20240504Large);

}  // namespace ken_ken
