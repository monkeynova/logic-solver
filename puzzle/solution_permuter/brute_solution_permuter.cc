#include "puzzle/solution_permuter/brute_solution_permuter.h"

#include "puzzle/class_permuter/factory.h"

namespace puzzle {

BruteSolutionPermuter::Advancer::Advancer(
    const BruteSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : AdvancerBase(entry_descriptor), permuter_(permuter) {
  if (entry_descriptor == nullptr) {
    set_done();
    return;
  }

  class_types_.resize(entry_descriptor->AllClasses()->size());
  std::iota(class_types_.begin(), class_types_.end(), 0);

  iterators_.resize(class_types_.size());
  for (auto class_int : class_types_) {
    iterators_[class_int] = permuter_->class_permuters_[class_int]->begin();
    mutable_solution().SetClass(iterators_[class_int]);
  }

  set_position({
      .position = 0,
      .count = permuter_->permutation_count(),
  });
}

void BruteSolutionPermuter::Advancer::Advance() {
  bool at_end = true;
  for (auto it = class_types_.rbegin(); it != class_types_.rend(); ++it) {
    int class_int = *it;
    ++iterators_[class_int];

    bool carry = false;
    if (iterators_[class_int] ==
        permuter_->class_permuters_[class_int]->end()) {
      iterators_[class_int] = permuter_->class_permuters_[class_int]->begin();
      carry = true;
    }

    mutable_solution().SetClass(iterators_[class_int]);

    if (!carry) {
      at_end = false;
      break;
    }
  }
  if (at_end) {
    set_done();
    set_position({
        .position = permuter_->permutation_count(),
        .count = permuter_->permutation_count(),
    });
  } else {
    set_position({
        .position = current().position().position + 1,
        .count = permuter_->permutation_count(),
    });
  }
}

Position BruteSolutionPermuter::Advancer::position() const {
  if (permuter_ == nullptr) {
    return {.position = 0, .count = 0};
  }

  double position = 0;

  for (int class_int : class_types_) {
    position *= permuter_->class_permuters_[class_int]->permutation_count();
    position += iterators_[class_int].position();
  }

  return {
      .position = position,
      .count = permuter_->permutation_count(),
  };
}

BruteSolutionPermuter::BruteSolutionPermuter(const EntryDescriptor* e)
    : SolutionPermuter(e) {}

absl::Status BruteSolutionPermuter::PrepareCheap() {
  int num_classes = entry_descriptor()->AllClasses()->size();
  class_permuters_.resize(num_classes);
  for (int class_int = 0; class_int < num_classes; ++class_int) {
    const Descriptor* class_descriptor =
        entry_descriptor()->AllClassValues(class_int);
    class_permuters_[class_int] =
        MakeClassPermuter(class_descriptor, class_int);
  }
  return absl::OkStatus();
}

absl::Status BruteSolutionPermuter::PrepareFull() { return absl::OkStatus(); }

double BruteSolutionPermuter::permutation_count() const {
  double count = 1;
  for (const auto& permuter : class_permuters_) {
    count *= permuter->permutation_count();
  }
  return count;
}

}  // namespace puzzle
