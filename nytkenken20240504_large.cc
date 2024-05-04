#include "puzzle/problem.h"
#include "vlog.h"

class NYTKenKen20240504Large : public puzzle::Problem {
 public:
  NYTKenKen20240504Large() : puzzle::Problem(MakeEntryDescriptor()) {}

 private:
  static int Val(const puzzle::Solution& s, int x, int y) { return s.Id(x).Class(y) + 1; }
  static int Val(const puzzle::Entry& e, int y) { return e.Class(y) + 1; }

  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  static puzzle::EntryDescriptor MakeEntryDescriptor();
};

puzzle::EntryDescriptor NYTKenKen20240504Large::MakeEntryDescriptor() {
  std::vector<std::string> id_names(6);
  for (int i = 0; i < 6; ++i) {
    id_names[i] = ("");
  }

  std::vector<std::unique_ptr<const puzzle::Descriptor>> class_descriptors;
  for (int i = 0; i < 6; ++i) {
    std::vector<std::string> class_names(6);
    for (int j = 0; j < 6; ++j) {
      class_names[j] = absl::StrCat(j + 1);
    }
    class_descriptors.push_back(
        absl::make_unique<puzzle::StringDescriptor>(std::move(class_names)));
  }

  return puzzle::EntryDescriptor(
      absl::make_unique<puzzle::StringDescriptor>(id_names),
      /*class_descriptor=*/
      absl::make_unique<puzzle::StringDescriptor>(
          std::vector<std::string>(6, "")),
      std::move(class_descriptors));
}

absl::Status NYTKenKen20240504Large::Setup() {
  std::vector<int> cols = {0};
  for (int i = 1; i < 6; ++i) {
    cols.push_back(i);
    RETURN_IF_ERROR(AddAllEntryPredicate(
        absl::StrCat("No row dupes ", i + 1),
        [i](const puzzle::Entry& e) {
          for (int j = 0; j < i; ++j) {
            if (e.Class(i) == e.Class(j)) return false;
          }
          return true;
        },
        cols));
  }
  
  RETURN_IF_ERROR(AddPredicate(
    "Box 1", [](const puzzle::Solution& s) {
      return Val(s, 0, 0) + Val(s, 1, 0) == 8;
    },
    {0});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 2", [](const puzzle::Solution& s) {
      return Val(s, 0, 1) * Val(s, 0, 2) == 10;
    },
    {1, 2});
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

  RETURN_IF_ERROR(AddPredicate(
    "Box 5", [](const puzzle::Solution& s) {
      return Val(s, 1, 1) - Val(s, 1, 2) == 1 || Val(s, 1, 2) - Val(s, 1, 1) == 1;
    },
    {1, 2});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 6", [](const puzzle::Solution& s) {
      return Val(s, 2, 0) - Val(s, 2, 1) == 2 || Val(s, 2, 1) - Val(s, 2, 0) == 2;
    },
    {0, 1});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 7", [](const puzzle::Solution& s) {
      return Val(s, 2, 2) + Val(s, 2, 3) + Val(s, 2, 4) == 6;
    },
    {2, 3, 4});
  );

  RETURN_IF_ERROR(AddPredicate(
    "Box 8", [](const puzzle::Solution& s) {
      return Val(s, 3, 0) == 2 * Val(s, 3, 1) || Val(s, 3, 1) == 2 * Val(s, 3, 0);
    },
    {0, 1});
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

  RETURN_IF_ERROR(AddPredicate(
    "Box 11", [](const puzzle::Solution& s) {
      return Val(s, 3, 4) - Val(s, 3, 5) == 2 || Val(s, 3, 5) - Val(s, 3, 4) == 2;
    },
    {4, 5});
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

  RETURN_IF_ERROR(AddPredicate(
    "Box 14", [](const puzzle::Solution& s) {
      return Val(s, 5, 2) == 1;
    },
    {2});
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
