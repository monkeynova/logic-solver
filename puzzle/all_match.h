#ifndef PUZZLE_ALL_MATCH_H
#define PUZZLE_ALL_MATCH_H

#include <vector>

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

inline bool AllMatch(const std::vector<SolutionFilter>& predicates,
                     const Solution& solution,
                     ClassPermuter::iterator::ValueSkip* value_skip = nullptr) {
  if (value_skip == nullptr) {
    return std::all_of(
        predicates.begin(), predicates.end(),
        [&solution](const SolutionFilter& c) { return c(solution); });
  }
  for (const SolutionFilter& filter : predicates) {
    if (!filter(solution)) {
      value_skip->value_index = filter.entry_id();
      return false;
    }
  }
  value_skip->value_index = Entry::kBadId;
  return true;
}

};  // namespace puzzle

#endif  // PUZZLE_ALL_MATCH_H
