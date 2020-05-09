#ifndef PUZZLE_SOLUTION_FILTER_H
#define PUZZLE_SOLUTION_FILTER_H

#include <limits>
#include <string>
#include <vector>

#include "puzzle/solution.h"

namespace puzzle {

class EntryFilter {
  EntryFilter() = default;
  EntryFilter(std::string name, Entry::Predicate p, std::vector<int> classes)
      : name_(std::move(name)), p_(p), classes_(std::move(classes)) {}

  bool operator()(const Entry& e) const { return p_(e); }

  absl::string_view name() const { return name_; }
  const std::vector<int>& classes() const { return classes_; }

 private:
  std::string name_;
  Entry::Predicate p_;
  std::vector<int> classes_;
};

class SolutionFilter {
 public:
  SolutionFilter() = default;
  SolutionFilter(std::string name, Solution::Predicate p,
                 std::vector<int> classes)
      : name_(std::move(name)), solution_p_(p), classes_(std::move(classes)) {}

  SolutionFilter(std::string name, int entry_id, Entry::Predicate p,
                 std::vector<int> classes)
      : name_(std::move(name)),
        solution_p_(
            [entry_id, p](const Solution& s) { return p(s.Id(entry_id)); }),
        classes_(std::move(classes)),
        entry_id_(entry_id) {}

  bool operator()(const Solution& s) const { return solution_p_(s); }

  bool operator()(const Entry& e) const {
    DCHECK_NE(entry_id_, std::numeric_limits<int>::max()) << name_;
    return entry_p_(e);
  }

  absl::string_view name() const { return name_; }
  const std::vector<int>& classes() const { return classes_; }

  int entry_id() const { return entry_id_; }

 private:
  std::string name_;
  Solution::Predicate solution_p_;
  std::vector<int> classes_;
  int entry_id_ = std::numeric_limits<int>::max();
  Entry::Predicate entry_p_;
};

inline bool AllMatch(const std::vector<SolutionFilter>& predicates,
		     const Solution& solution) {
  return std::all_of(predicates.begin(), predicates.end(),
                     [&solution](const SolutionFilter& c) { return c(solution); });
}

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_FILTER_H
