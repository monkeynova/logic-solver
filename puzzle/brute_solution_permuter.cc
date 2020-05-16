#include "puzzle/brute_solution_permuter.h"

namespace puzzle {

BruteSolutionPermuter::Advancer::Advancer(
    const BruteSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : permuter_(permuter), mutable_solution_(entry_descriptor) {
  if (entry_descriptor == nullptr) {
    return;
  }

  class_types_ = entry_descriptor->AllClasses()->Values();

  iterators_.resize(class_types_.size());
  for (auto class_int : class_types_) {
    iterators_[class_int] = permuter_->class_permuters_[class_int]->begin();
    mutable_solution_.SetClass(iterators_[class_int]);
  }

  current_ = mutable_solution_.TestableSolution();
  current_.set_permutation_count(permuter_->permutation_count());
  current_.set_permutation_position(0);
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

    mutable_solution_.SetClass(iterators_[class_int]);

    if (!carry) {
      at_end = false;
      break;
    }
  }
  if (at_end) {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  } else {
    current_.set_permutation_position(current_.permutation_position() + 1);
  }
}

double BruteSolutionPermuter::Advancer::position() const {
  double position = 0;

  for (int class_int : class_types_) {
    position *= permuter_->class_permuters_[class_int]->permutation_count();
    position += iterators_[class_int].position();
  }

  return position;
}

double BruteSolutionPermuter::Advancer::completion() const {
  return position() / permuter_->permutation_count();
}

BruteSolutionPermuter::BruteSolutionPermuter(const EntryDescriptor* e)
    : entry_descriptor_(e) {}

void BruteSolutionPermuter::Prepare() {
  const std::vector<int>& class_types =
      entry_descriptor_->AllClasses()->Values();

  class_permuters_.resize(class_types.size());
  for (auto class_int : class_types) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_[class_int] =
        MakeClassPermuter(class_descriptor, class_int);
  }
}

double BruteSolutionPermuter::permutation_count() const {
  double count = 1;
  for (const auto& permuter : class_permuters_) {
    count *= permuter->permutation_count();
  }
  return count;
}

}  // namespace puzzle
