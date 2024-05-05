#include "ken_ken/board.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace KenKen {

class NYTKenKen20240504Small : public Board<4> {
 public:
  NYTKenKen20240504Small() = default;

 private:
  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

absl::Status NYTKenKen20240504Small::Setup() {
  RETURN_IF_ERROR(AddBoardPredicates());

  RETURN_IF_ERROR(AddPredicate(
    "Box 1", [](const puzzle::Solution& s) {
      return Val(s, 0, 0) + Val(s, 1, 0) == 7;
    },
    {0});
  );

  if (true) {
    RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "Box 2", [](const puzzle::Entry& e) {
        return Val(e, 1) + Val(e, 2) + Val(e, 3) == 7;
      },
      {1, 2, 3}, 0);
    );
  } else {
    RETURN_IF_ERROR(AddPredicate(
      "Box 2", [](const puzzle::Solution& s) {
        return Val(s, 0, 1) + Val(s, 0, 2) + Val(s, 0, 3) == 7;
      },
      {1, 2, 3});
    );
  }

  RETURN_IF_ERROR(AddPredicate(
    "Box 3", [](const puzzle::Solution& s) {
      return Val(s, 1, 1) + Val(s, 2, 0) + Val(s, 2, 1) + Val(s, 3, 0) == 10;
    },
    {0, 1});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 4", [](const puzzle::Solution& s) {
      return Val(s, 1, 2) + Val(s, 1, 3) + Val(s, 2, 3) == 6;
    },
    {2, 3});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 5", [](const puzzle::Solution& s) {
      return Val(s, 2, 2) == 1;
    },
    {2});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 6", [](const puzzle::Solution& s) {
      return Val(s, 3, 1) + Val(s, 3, 2) + Val(s, 3, 3) == 9;
    },
    {1, 2, 3});
  );

  return absl::OkStatus();
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
