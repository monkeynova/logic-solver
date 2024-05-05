#include "ken_ken/board.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace KenKen {

class NYTKenKen20240504Small : public Board<4> {
 public:
  NYTKenKen20240504Small() = default;

  std::vector<Cage> GetCages() const override;

 private:
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

std::vector<Board<4>::Cage> NYTKenKen20240504Small::GetCages() const {
  return std::vector<Cage>{
      {7, Cage::kAdd, {{0, 0}, {1, 0}}},
      {7, Cage::kAdd, {{0, 1}, {0, 2}, {0, 3}}},
      {10, Cage::kAdd, {{1, 1}, {2, 0}, {2, 1}, {3, 0}}},
      {6, Cage::kAdd, {{1, 2}, {1, 3}, {2, 3}}},
      {1, Cage::kAdd, {{2, 2}}},
      {9, Cage::kAdd, {{3, 1}, {3, 2}, {3, 3}}},
  };
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

}  // namespace KenKen
