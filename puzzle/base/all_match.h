#ifndef PUZZLE_BASE_ALL_MATCH_H
#define PUZZLE_BASE_ALL_MATCH_H

#include <vector>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"
#include "puzzle/base/solution_filter.h"
#include "puzzle/base/solution_view.h"

namespace puzzle {

// Returns true if all entries in `predicates` are true for `solution`.
// If `value_skip` is non-nullptr, returns the entry_id for the corresponding
// `class_int` on the first predicate that evaluates to false.
inline bool AllMatch(absl::Span<const SolutionFilter> predicates,
                     const SolutionView& solution) {
  return absl::c_all_of(predicates,
                        [&](const SolutionFilter& c) { return c(solution); });
}

// Argument type for operator+= to advance until a sepecific position in the
// permutation changes values.
struct ValueSkip {
  int value_index = Entry::kBadId;
};

static_assert(sizeof(ValueSkip) < 16,
              "ValueSkip is assumed to be small enough for pass-by-value "
              "semantics.");

// Returns true if all entries in `predicates` are true for `solution`.
// If `value_skip` is non-nullptr, returns the entry_id for the corresponding
// `class_int` on the first predicate that evaluates to false.
inline bool AllMatch(absl::Span<const SolutionFilter> predicates,
                     const SolutionView& solution, int class_int,
                     ValueSkip& value_skip) {
  for (const SolutionFilter& filter : predicates) {
    if (!filter(solution)) {
      value_skip.value_index = filter.entry_id(class_int);
      return false;
    }
  }
  value_skip.value_index = Entry::kBadId;
  return true;
}

// Returns a bit vector containint all entry_ids at `class_int` for entries in
// `predicates` that evaluate to false on `solution`.
inline int UnmatchedEntrySkips(absl::Span<const SolutionFilter> predicates,
                               const SolutionView& solution,
                               int class_int = -1) {
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

#endif  // PUZZLE_BASE_ALL_MATCH_H
