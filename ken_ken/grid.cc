#include "ken_ken/grid.h"

namespace ken_ken {

template <int64_t kWidth>
puzzle::EntryDescriptor Grid<kWidth>::MakeEntryDescriptor() {
  std::vector<std::string> id_names(kWidth);
  for (int i = 0; i < kWidth; ++i) {
    id_names[i] = ("");
  }

  std::vector<std::unique_ptr<const puzzle::Descriptor>> class_descriptors;
  for (int i = 0; i < kWidth; ++i) {
    std::vector<std::string> class_names(kWidth);
    for (int j = 0; j < kWidth; ++j) {
      class_names[j] = absl::StrCat(j + 1);
    }
    class_descriptors.push_back(
        absl::make_unique<puzzle::StringDescriptor>(std::move(class_names)));
  }

  return puzzle::EntryDescriptor(
      absl::make_unique<puzzle::StringDescriptor>(id_names),
      /*class_descriptor=*/
      absl::make_unique<puzzle::StringDescriptor>(
          std::vector<std::string>(kWidth, "")),
      std::move(class_descriptors));
}

template <int64_t kWidth>
absl::Status Grid<kWidth>::Setup() {
  for (int i = 0; i < kWidth; ++i) {
    for (int j = i + 1; j < kWidth; ++j) {
      absl::Status st = AddAllEntryPredicate(
          absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
          [i, j](const puzzle::Entry& e) { return e.Class(i) != e.Class(j); },
          {i, j});
      if (!st.ok()) return st;
    }
  }

  RETURN_IF_ERROR(AddGridPredicates());
  return absl::OkStatus();
}

template class Grid<4>;
template class Grid<6>;
template class Grid<9>;

}  // namespace ken_ken
