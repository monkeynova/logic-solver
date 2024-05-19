#include "ken_ken/grid.h"

#include "absl/flags/flag.h"

ABSL_FLAG(std::string, sudoku_problem_setup, "pairwise",
          "Sepecifies the form of the predicates passed to the puzzle "
          "solver to validate sudoku boards. Valid vaules are 'cumulative' "
          "and 'pairwise'. 'cumulative' is faster if predicate reordering "
          "is disabled, but 'pairwise' is better suited for predicate "
          "reordering and results in faster overall evaluation if "
          "reordering is enabled.");

ABSL_FLAG(bool, grid_try_transpose, true, "...");

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
  default_id_ = DefaultAlternate();
  if (absl::GetFlag(FLAGS_grid_try_transpose)) {
    ASSIGN_OR_RETURN(transpose_id_, CreateAlternate());
  }

  if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "cumulative") {
    std::vector<int> cols = {0};
    for (int i = 1; i < 9; ++i) {
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
  } else if (absl::GetFlag(FLAGS_sudoku_problem_setup) == "pairwise") {
    for (int i = 0; i < kWidth; ++i) {
      for (int j = i + 1; j < kWidth; ++j) {
        RETURN_IF_ERROR(AddAllEntryPredicate(
            absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
            [i, j](const puzzle::Entry& e) { return e.Class(i) != e.Class(j); },
            {i, j}));
      }
    }
  }

  SetAlternate(default_id_);
  RETURN_IF_ERROR(AddGridPredicates(Orientation::kDefault));
  if (absl::GetFlag(FLAGS_grid_try_transpose)) {
    SetAlternate(transpose_id_);
    RETURN_IF_ERROR(AddGridPredicates(Orientation::kTranspose));
  }
  SetAlternate(std::nullopt);

  return absl::OkStatus();
}

template <int64_t kWidth>
absl::StatusOr<puzzle::Solution> Grid<kWidth>::TransformAlternate(
    puzzle::Solution in, AlternateId alternate) const {
  if (alternate == transpose_id_) {
    if (!absl::GetFlag(FLAGS_grid_try_transpose)) {
      return absl::InternalError("Transpose disabled");
    }
    std::vector<puzzle::Entry> copy = in.entries();
    for (int i = 0; i < kWidth; ++i) {
      for (int j = 0; j < kWidth; ++j) {
        copy[j].SetClass(i, in.Id(i).Class(j));
      }
    }
    return puzzle::Solution(in.descriptor(), &copy).Clone();
  }
  if (alternate != default_id_) {
    return absl::InternalError("Bad alternate");
  }
  return in;
}

template class Grid<4>;
template class Grid<6>;
template class Grid<9>;

}  // namespace ken_ken
