#include "puzzle/problem.h"
#include "vlog.h"

class NYTKenKen20240504Small : public puzzle::Problem {
 public:
  NYTKenKen20240504Small() : puzzle::Problem(MakeEntryDescriptor()) {}

 private:
  static int Val(const puzzle::Solution& s, int x, int y) { return s.Id(x).Class(y) + 1; }
  static int Val(const puzzle::Entry& e, int y) { return e.Class(y) + 1; }

  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  static puzzle::EntryDescriptor MakeEntryDescriptor();
};

puzzle::EntryDescriptor NYTKenKen20240504Small::MakeEntryDescriptor() {
  std::vector<std::string> id_names(4);
  for (int i = 0; i < 4; ++i) {
    id_names[i] = ("");
  }

  std::vector<std::unique_ptr<const puzzle::Descriptor>> class_descriptors;
  for (int i = 0; i < 4; ++i) {
    std::vector<std::string> class_names(4);
    for (int j = 0; j < 4; ++j) {
      class_names[j] = absl::StrCat(j + 1);
    }
    class_descriptors.push_back(
        absl::make_unique<puzzle::StringDescriptor>(std::move(class_names)));
  }

  return puzzle::EntryDescriptor(
      absl::make_unique<puzzle::StringDescriptor>(id_names),
      /*class_descriptor=*/
      absl::make_unique<puzzle::StringDescriptor>(
          std::vector<std::string>(4, "")),
      std::move(class_descriptors));
}

absl::Status NYTKenKen20240504Small::Setup() {
  std::vector<int> cols = {0};
  for (int i = 1; i < 4; ++i) {
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
