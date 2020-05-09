#ifndef PUZZLE_SOLUTION_FILTER_H
#define PUZZLE_SOLUTION_FILTER_H

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
      : name_(std::move(name)), p_(p), classes_(std::move(classes)) {}

  SolutionFilter(std::string name, int entry_id, Entry::Predicate p,
                 std::vector<int> classes)
      : SolutionFilter(
            std::move(name),
            [entry_id, p](const Solution& s) { return p(s.Id(entry_id)); },
            std::move(classes)) {}

  bool operator()(const Solution& s) const { return p_(s); }

  absl::string_view name() const { return name_; }
  const std::vector<int>& classes() const { return classes_; }

 private:
  std::string name_;
  Solution::Predicate p_;
  std::vector<int> classes_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_FILTER_H