#ifndef PUZZLE_MUTABLE_SOLUTION_H_
#define PUZZLE_MUTABLE_SOLUTION_H_

#include <vector>

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"

namespace puzzle {

class MutableSolution {
 public:
  explicit MutableSolution(const EntryDescriptor* descriptor)
      : descriptor_(descriptor) {
    if (descriptor != nullptr) {
      const int num_classes = descriptor_->num_classes();

      std::vector<int> invalid_classes(num_classes, -1);

      entries_.reserve(num_classes);
      for (int id = 0; id < descriptor_->AllIds()->size(); ++id) {
        entries_.push_back(Entry(id, invalid_classes, descriptor_));
      }
    }
  }

  Solution TestableSolution() { return Solution(descriptor_, &entries_); }

  // TODO(@monkeynova): Allow a MutableSolution to bind to class
  // permuters and pull values from them directly avoiding this push model.
  void SetClass(const ClassPermuter::iterator& it) {
    for (unsigned int j = 0; j < it->size(); ++j) {
      entries_[j].SetClass(it.class_int(), (*it)[j]);
    }
  }

 private:
  const EntryDescriptor* descriptor_;
  std::vector<Entry> entries_;
};

}  // namespace puzzle

#endif  // PUZZLE_MUTABLE_SOLUTION_H_
