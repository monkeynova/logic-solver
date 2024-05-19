#include "sudoku/greater_than_sudoku.h"

#include "absl/flags/flag.h"

ABSL_FLAG(bool, sudoku_greater_than_composition, true,
          "If true, adds predicates not just on the greater-thans themselves, "
          "but also follows the greater-than graph to add range limits on "
          "individual cells values.");

namespace sudoku {

absl::Status GreaterThanSudoku::AddComparison(const std::pair<Box, Box>& cmp) {
  if (abs(cmp.first.entry_id - cmp.second.entry_id) +
          abs(cmp.first.class_id - cmp.second.class_id) !=
      1) {
    return absl::InvalidArgumentError(
        "Comparison must be on neighboring boxes");
  }
  if (cmp.first.entry_id / 3 != cmp.second.entry_id / 3) {
    return absl::InvalidArgumentError(
        "Comparisons must be with in a box (entry)");
  }
  if (cmp.first.class_id / 3 != cmp.second.class_id / 3) {
    return absl::InvalidArgumentError(
        "Comparisons must be with in a box (class)");
  }
  if (cmp.first.entry_id == cmp.second.entry_id) {
    return AddSpecificEntryPredicate(
        absl::StrCat(cmp.first, " > ", cmp.second),
        [cmp](const puzzle::Entry& e) {
          return e.Class(cmp.first.class_id) > e.Class(cmp.second.class_id);
        },
        {cmp.first.class_id, cmp.second.class_id}, cmp.first.entry_id);
  }
  if (cmp.first.class_id != cmp.second.class_id) {
    return absl::InvalidArgumentError(
        "Comparison must be on neighboring boxes");
  }
  return AddPredicate(
      absl::StrCat(cmp.first, " > ", cmp.second),
      [cmp](const puzzle::Solution& s) {
        return s.Id(cmp.first.entry_id).Class(cmp.first.class_id) >
               s.Id(cmp.second.entry_id).Class(cmp.second.class_id);
      },
      {cmp.first.class_id});
}

absl::Status GreaterThanSudoku::InstanceSetup(
    ::ken_ken::Grid<kWidth>::Orientation o) {
  std::vector<std::pair<Box, Box>> cmp_list = GetComparisons();

  switch (o) {
    case ::ken_ken::Grid<kWidth>::Orientation::kDefault:
      break;
    case ::ken_ken::Grid<kWidth>::Orientation::kTranspose: {
      for (auto& [b1, b2] : cmp_list) {
        b1.Transpose();
        b2.Transpose();
      }
      break;
    }
  }

  // Add the pairwise comparisons.
  for (const std::pair<Box, Box>& cmp : cmp_list) {
    RETURN_IF_ERROR(AddComparison(cmp));
  }

  if (absl::GetFlag(FLAGS_sudoku_greater_than_composition)) {
    RETURN_IF_ERROR(AddRangePredicates(cmp_list));
  }

  return absl::OkStatus();
}

absl::Status GreaterThanSudoku::AddRangePredicates(
    const std::vector<std::pair<Box, Box>>& cmp_list) {
  // Build a directed graph of the comparisons.
  absl::flat_hash_map<Box, std::vector<Box>> greater_than_map;
  absl::flat_hash_map<Box, std::vector<Box>> less_than_map;
  absl::flat_hash_set<Box> all;
  for (const std::pair<Box, Box>& cmp : cmp_list) {
    all.insert(cmp.first);
    all.insert(cmp.second);
    less_than_map[cmp.first].push_back(cmp.second);
    greater_than_map[cmp.second].push_back(cmp.first);
  }

  // For each point in the graph, count the points before and after it in the
  // DAG. The number of those points limit the range of the value at the point.
  for (Box b : all) {
    absl::flat_hash_set<Box> prev;
    std::vector<Box> frontier = {b};
    while (!frontier.empty()) {
      Box t1 = frontier.back();
      frontier.pop_back();
      for (Box t2 : less_than_map[t1]) {
        if (prev.contains(t2)) continue;
        prev.insert(t2);
        frontier.push_back(t2);
      }
    }

    absl::flat_hash_set<Box> next;
    frontier = {b};
    while (!frontier.empty()) {
      Box t1 = frontier.back();
      frontier.pop_back();
      for (Box t2 : greater_than_map[t1]) {
        if (next.contains(t2)) continue;
        next.insert(t2);
        frontier.push_back(t2);
      }
    }

    if (prev.empty() && next.empty()) {
      // We only run over points we found in the graph. Either forward or back
      // must be non-empty to have been added.
      return absl::InternalError("No forward or backward found");
    }

    int prev_size = prev.size();
    int next_size = next.size();
    RETURN_IF_ERROR(AddSpecificEntryPredicate(
        absl::StrCat("(", b.entry_id, ",", b.class_id, ") chain; ",
                     "prev=", prev_size, "; next=", next_size),
        [b, prev_size, next_size](const puzzle::Entry& e) {
          // Class is 0-indexed.
          return e.Class(b.class_id) >= prev_size &&
                 e.Class(b.class_id) <= 8 - next_size;
        },
        {b.class_id}, b.entry_id));
  }

  return absl::OkStatus();
}

}  // namespace sudoku
