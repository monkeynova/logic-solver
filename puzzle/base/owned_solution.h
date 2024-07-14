#ifndef PUZZLE_BASE_OWNED_SOLUTION_H
#define PUZZLE_BASE_OWNED_SOLUTION_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "puzzle/base/descriptors.h"
#include "puzzle/base/position.h"
#include "puzzle/base/solution_view.h"

namespace puzzle {

class OwnedSolution {
 public:
  explicit OwnedSolution(SolutionView view)
      : entry_descriptor_(view.descriptor()),
        entries_(view.entries()),
        permutation_position_(view.position()) {}
  OwnedSolution(const EntryDescriptor* entry_descriptor,
                std::vector<Entry> entries)
      : entry_descriptor_(entry_descriptor), entries_(std::move(entries)) {}

  OwnedSolution(const OwnedSolution& other) = delete;
  OwnedSolution& operator=(const OwnedSolution& other) = delete;

  OwnedSolution(OwnedSolution&& other) = default;
  OwnedSolution& operator=(OwnedSolution&& other) = default;

  SolutionView view() const {
    SolutionView view(entry_descriptor_, &entries_);
    view.set_position(permutation_position_);
    return view;
  }

  bool operator==(const OwnedSolution& o) const {
    return entries_ == o.entries_;
  }

  bool IsValid() const { return view().IsValid(); }
  Position position() const { return permutation_position_; }

  template <typename Sink>
  friend void AbslStringify(Sink& s, const OwnedSolution& os) {
    absl::Format(&s, "%v", os.view());
  }

  friend std::ostream& operator<<(std::ostream& o, const OwnedSolution& os) {
    return o << absl::StreamFormat("%v", os);
  }

 private:
  const EntryDescriptor* entry_descriptor_ = nullptr;  // Not owned

  std::vector<Entry> entries_;

  // The position of in iterating through all permutations of solutions which
  // this represents.
  Position permutation_position_ = {.position = 0, .count = 0};
};

}  // namespace puzzle

#endif  // PUZZLE_BASE_OWNED_SOLUTION_H