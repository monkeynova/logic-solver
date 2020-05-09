#ifndef PUZZLE_SOLUTION_FILTER_H
#define PUZZLE_SOLUTION_FILTER_H

#include <string>
#include <vector>

#include "puzzle/solution.h"

namespace puzzle {

class SolutionFilter {
 public:
  SolutionFilter() = default;
  SolutionFilter(std::string name, Solution::Predicate p,
                 std::vector<int> classes)
      : name_(std::move(name)), p_(p), classes_(std::move(classes)) {}

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
