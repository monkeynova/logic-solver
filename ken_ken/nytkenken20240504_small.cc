#include "ken_ken/board.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace ken_ken {

class NYTKenKen20240504Small : public Board<4> {
 public:
  NYTKenKen20240504Small() = default;

  absl::StatusOr<std::string_view> GetCageBoard() const override;

 private:
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

absl::StatusOr<std::string_view> NYTKenKen20240504Small::GetCageBoard() const {
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

absl::StatusOr<puzzle::Solution> NYTKenKen20240504Small::GetSolution() const {
  std::vector<puzzle::Entry> entries;
  entries.emplace_back(0, std::vector<int>{2, 0, 3, 1}, entry_descriptor());
  entries.emplace_back(1, std::vector<int>{3, 2, 1, 0}, entry_descriptor());
  entries.emplace_back(2, std::vector<int>{1, 3, 0, 2}, entry_descriptor());
  entries.emplace_back(3, std::vector<int>{0, 1, 2, 3}, entry_descriptor());
  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

REGISTER_PROBLEM(NYTKenKen20240504Small);

}  // namespace ken_ken
