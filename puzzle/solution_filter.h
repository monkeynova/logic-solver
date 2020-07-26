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
      : name_(std::move(name)), solution_p_(p), classes_(std::move(classes)) {}

  SolutionFilter(std::string name, Solution::Predicate p,
                 std::initializer_list<int> classes)
      : SolutionFilter(std::move(name), p, std::vector<int>(classes)) {}

  SolutionFilter(std::string name, Entry::Predicate p, std::vector<int> classes,
                 int entry_id)
      : name_(std::move(name)),
        solution_p_(
            [entry_id, p](const Solution& s) { return p(s.Id(entry_id)); }),
        classes_(std::move(classes)),
        entry_id_(entry_id) {}

  SolutionFilter(std::string name, Solution::Predicate p,
                 absl::flat_hash_map<int, int> class_to_entry)
      : name_(std::move(name)),
        solution_p_(p),
        class_to_entry_(std::move(class_to_entry)) {
    for (const auto& class_and_entry : class_to_entry_) {
      classes_.push_back(class_and_entry.first);
    }
  }

  static std::function<bool(const SolutionFilter& a, const SolutionFilter& b)>
  LtByEntryId(int class_int = -1) {
    return [class_int](const SolutionFilter& a, const SolutionFilter& b) {
      return a.entry_id(class_int) < b.entry_id(class_int);
    };
  }

  bool operator()(const Solution& s) const { return solution_p_(s); }

  bool operator()(const Entry& e) const {
    DCHECK_NE(entry_id_, Entry::kBadId) << name_;
    return entry_p_(e);
  }

  absl::string_view name() const { return name_; }
  const std::vector<int>& classes() const { return classes_; }

  int entry_id(int class_int) const {
    if (entry_id_ != Entry::kBadId) return entry_id_;
    auto it = class_to_entry_.find(class_int);
    if (it != class_to_entry_.end()) {
      return it->second;
    }
    return Entry::kBadId;
  }

 private:
  std::string name_;
  Solution::Predicate solution_p_;
  std::vector<int> classes_;
  int entry_id_ = Entry::kBadId;
  absl::flat_hash_map<int, int> class_to_entry_;
  Entry::Predicate entry_p_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_FILTER_H
