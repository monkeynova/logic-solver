#include "ken_ken/board.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace KenKen {

class NYTKenKen20240504Large : public Board<6> {
 public:
  NYTKenKen20240504Large() = default;

  std::vector<Cage> GetCages() const override;

 private:
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

std::vector<Board<6>::Cage> NYTKenKen20240504Large::GetCages() const {
  return std::vector<Cage>{
      {8, Cage::kAdd, {{0, 0}, {1, 0}}},
      {10, Cage::kMul, {{0, 1}, {0, 2}}},
      {12, Cage::kAdd, {{0, 3}, {0, 4}, {1, 3}}},
      {30, Cage::kMul, {{0, 5}, {1, 4}, {1, 5}, {2, 5}}},
      {1, Cage::kSub, {{1, 1}, {1, 2}}},
      {2, Cage::kSub, {{2, 0}, {2, 1}}},
      {6, Cage::kAdd, {{2, 2}, {2, 3}, {2, 4}}},
      {2, Cage::kDiv, {{3, 0}, {3, 1}}},
      {18, Cage::kAdd, {{3, 2}, {4, 1}, {4, 2}, {5, 1}}},
      {180, Cage::kMul, {{3, 3}, {4, 3}, {5, 3}, {5, 4}}},
      {2, Cage::kSub, {{3, 4}, {3, 5}}},
      {2, Cage::kDiv, {{4, 0}, {5, 0}}},
      {10, Cage::kAdd, {{4, 4}, {4, 5}, {5, 5}}},
      {1, Cage::kAdd, {{5, 2}}},
  };
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

}  // namespace KenKen
