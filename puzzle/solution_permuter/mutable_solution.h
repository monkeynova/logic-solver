#ifndef PUZZLE_SOLUTION_PERMUTER_MUTABLE_SOLUTION_H_
#define PUZZLE_SOLUTION_PERMUTER_MUTABLE_SOLUTION_H_

#include <vector>

#include "puzzle/base/solution.h"
#include "puzzle/class_permuter/class_permuter.h"

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
        entries_.push_back(Entry(id, invalid_classes));
      }
    }
  }

  Solution TestableSolution() { return Solution(descriptor_, &entries_); }

  // We push values from thee iteration to all values in the set. One could
  // imagine a mode where we directly bind the values from the class iterator
  // to the solution. In an experiment to do that the performance change was
  // negligable, so we leave this more readable form.
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

#endif  // PUZZLE_SOLUTION_PERMUTER_MUTABLE_SOLUTION_H_
