#ifndef KEN_KEB_BOARD_H
#define KEN_KEB_BOARD_H

#include <cstdint>

#include "puzzle/problem.h"

namespace KenKen {

template <int64_t kWidth>
class Board : public puzzle::Problem {
 public:
  Board() : puzzle::Problem(MakeEntryDescriptor()) {}

 protected:
  static int Val(const puzzle::Solution& s, int x, int y) { return s.Id(x).Class(y) + 1; }
  static int Val(const puzzle::Entry& e, int y) { return e.Class(y) + 1; }

  absl::Status AddBoardPredicates();

 private:
  static puzzle::EntryDescriptor MakeEntryDescriptor();
};

template <int64_t kWidth>
puzzle::EntryDescriptor Board<kWidth>::MakeEntryDescriptor() {
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
absl::Status Board<kWidth>::AddBoardPredicates() {
  std::vector<int> cols = {0};
  for (int i = 1; i < kWidth; ++i) {
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
  
  return absl::OkStatus();
}

}  // namespace KenKen

#endif  // KEN_KEB_BOARD_H