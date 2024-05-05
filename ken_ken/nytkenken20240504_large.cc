#include "ken_ken/board.h"
#include "puzzle/problem.h"
#include "vlog.h"

namespace KenKen {

class NYTKenKen20240504Large : public Board<6> {
 public:
  NYTKenKen20240504Large() = default;

 private:
  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;
};

absl::Status NYTKenKen20240504Large::Setup() {
  RETURN_IF_ERROR(AddBoardPredicates());
  
  RETURN_IF_ERROR(AddPredicate(
    "Box 1", [](const puzzle::Solution& s) {
      return Val(s, 0, 0) + Val(s, 1, 0) == 8;
    },
    {0});
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 2", [](const puzzle::Entry& e) {
      return Val(e, 1) * Val(e, 2) == 10;
    },
    {1, 2}, 0);
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 3", [](const puzzle::Solution& s) {
      return Val(s, 0, 3) + Val(s, 0, 4) + Val(s, 1, 3) == 12;
    },
    {3, 4});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 4", [](const puzzle::Solution& s) {
      return Val(s, 0, 5) * Val(s, 1, 4) * Val(s, 1, 5) * Val(s, 2, 5) == 30;
    },
    {5, 4});
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 5", [](const puzzle::Entry& e) {
      return Val(e, 1) - Val(e, 2) == 1 || Val(e, 2) - Val(e, 1) == 1;
    },
    {1, 2}, 1);
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 6", [](const puzzle::Entry& e) {
      return Val(e, 0) - Val(e, 1) == 2 || Val(e, 1) - Val(e, 0) == 2;
    },
    {0, 1}, 2);
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 7", [](const puzzle::Entry& e) {
      return Val(e, 2) + Val(e, 3) + Val(e, 4) == 6;
    },
    {2, 3, 4}, 2);
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 8", [](const puzzle::Entry& e) {
      return Val(e, 0) == 2 * Val(e, 1) || Val(e, 1) == 2 * Val(e, 0);
    },
    {0, 1}, 3);
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 9", [](const puzzle::Solution& s) {
      return Val(s, 3, 2) + Val(s, 4, 1) + Val(s, 4, 2) + Val(s, 5, 1) == 18;
    },
    {1, 2});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 10", [](const puzzle::Solution& s) {
      return Val(s, 3, 3) * Val(s, 4, 3) * Val(s, 5, 3) * Val(s, 5, 4) == 180;
    },
    {3, 4});
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 11", [](const puzzle::Entry& e) {
      return Val(e, 4) - Val(e, 5) == 2 || Val(e, 5) - Val(e, 4) == 2;
    },
    {4, 5}, 3);
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 12", [](const puzzle::Solution& s) {
      return Val(s, 4, 0) == 2 * Val(s, 5, 0) || Val(s, 5, 0) == 2 * Val(s, 4, 0);
    },
    {0});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 13", [](const puzzle::Solution& s) {
      return Val(s, 4, 4) + Val(s, 4, 5) + Val(s, 5, 5) == 10;
    },
    {4, 5});
  );

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
    "Box 14", [](const puzzle::Entry& e) {
      return Val(e, 2) == 1;
    },
    {2}, 5);
  );

  return absl::OkStatus();
}

absl::StatusOr<puzzle::Solution> NYTKenKen20240504Large::GetSolution() const {
  std::vector<puzzle::Entry> entries;
  entries.emplace_back(0, std::vector<int>{2, 4, 1, 5, 3, 0}, entry_descriptor());
  entries.emplace_back(1, std::vector<int>{4, 2, 3, 1, 0, 5}, entry_descriptor());
  entries.emplace_back(2, std::vector<int>{5, 3, 2, 0, 1, 4}, entry_descriptor());
  entries.emplace_back(3, std::vector<int>{0, 1, 4, 2, 5, 3}, entry_descriptor());
  entries.emplace_back(4, std::vector<int>{1, 0, 5, 3, 4, 2}, entry_descriptor());
  entries.emplace_back(5, std::vector<int>{3, 5, 0, 4, 2, 1}, entry_descriptor());
  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

REGISTER_PROBLEM(NYTKenKen20240504Large);

}  // namespace KenKen
