#ifndef PUZZLE_ALL_MATCH_H
#define PUZZLE_ALL_MATCH_H

#include <vector>

#include "absl/types/span.h"
#include "puzzle/base/solution.h"
#include "puzzle/base/solution_filter.h"
#include "puzzle/class_permuter/class_permuter.h"

namespace puzzle {

// Returns true if all entries in `predicates` are true for `solution`.
// If `value_skip` is non-nullptr, returns the entry_id for the corresponding
// `class_int` on the first predicate that evaluates to false.
inline bool AllMatch(absl::Span<const SolutionFilter> predicates,
                     const Solution& solution, int class_int = -1,
                     ClassPermuter::iterator::ValueSkip* value_skip = nullptr) {
  if (value_skip == nullptr) {
    return std::all_of(
        predicates.begin(), predicates.end(),
        [&solution](const SolutionFilter& c) { return c(solution); });
  }
  for (const SolutionFilter& filter : predicates) {
    if (!filter(solution)) {
      value_skip->value_index = filter.entry_id(class_int);
      return false;
    }
  }
  value_skip->value_index = Entry::kBadId;
  return true;
}

// Returns a bit vector containint all entry_ids at `class_int` for entries in
// `predicates` that evaluate to false on `solution`.
inline int UnmatchedEntrySkips(absl::Span<const SolutionFilter> predicates,
                               const Solution& solution, int class_int = -1) {
  int all_entry_skips = 0;
  for (const SolutionFilter& filter : predicates) {
    if (!filter(solution)) {
      int entry_id = filter.entry_id(class_int);
      if (entry_id != Entry::kBadId) {
        all_entry_skips |= 1 << entry_id;
      }
    }
  }
  return all_entry_skips;
}

};  // namespace puzzle

#endif  // PUZZLE_ALL_MATCH_H
