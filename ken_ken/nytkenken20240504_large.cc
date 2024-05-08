#include "ken_ken/ken_ken.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace ken_ken {

class NYTKenKen20240504Large : public KenKen<6> {
 public:
  NYTKenKen20240504Large() = default;

  absl::StatusOr<std::string_view> GetCageKenKen() const override;

 private:
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
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

absl::StatusOr<puzzle::Solution> NYTKenKen20240504Large::GetSolution() const {
  std::vector<puzzle::Entry> entries;
  entries.emplace_back(0, std::vector<int>{2, 4, 1, 5, 3, 0},
                       entry_descriptor());
  entries.emplace_back(1, std::vector<int>{4, 2, 3, 1, 0, 5},
                       entry_descriptor());
  entries.emplace_back(2, std::vector<int>{5, 3, 2, 0, 1, 4},
                       entry_descriptor());
  entries.emplace_back(3, std::vector<int>{0, 1, 4, 2, 5, 3},
                       entry_descriptor());
  entries.emplace_back(4, std::vector<int>{1, 0, 5, 3, 4, 2},
                       entry_descriptor());
  entries.emplace_back(5, std::vector<int>{3, 5, 0, 4, 2, 1},
                       entry_descriptor());
  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

REGISTER_PROBLEM(NYTKenKen20240504Large);

}  // namespace ken_ken
