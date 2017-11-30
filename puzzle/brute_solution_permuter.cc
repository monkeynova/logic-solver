#include "puzzle/brute_solution_permuter.h"

namespace Puzzle {

BruteSolutionPermuter::iterator::iterator(
   const BruteSolutionPermuter* permuter, 
   const EntryDescriptor* entry_descriptor)
   : permuter_(permuter), entry_descriptor_(entry_descriptor) {
  if (entry_descriptor_ == nullptr) {
    return;
  }
  
  class_types_ = entry_descriptor->AllClasses()->Values();
  
  std::vector<int> bad_classes(class_types_.size(), -1);
  
  for (auto id: entry_descriptor_->AllIds()->Values()) {
    entries_.push_back(Entry(id,bad_classes,entry_descriptor_));
  }
  
  iterators_.resize(class_types_.size());
  for (auto class_int: class_types_) {
    iterators_[class_int] = permuter_->class_permuters_[class_int].begin();
    
    const std::vector<int>& class_values = *(iterators_[class_int]);
    for (unsigned int j = 0; j < class_values.size(); j++ ) {
      entries_[j].SetClass(class_int, class_values[j]);
    }
  }
  
  current_ = Solution(&entries_);
  current_.set_permutation_count(permuter_->permutation_count());
  current_.set_permutation_position(0);
}

void BruteSolutionPermuter::iterator::Advance() {
  bool at_end = true;
  for (int class_int: class_types_) {
    ++iterators_[class_int];
    
    bool carry = false;
    if (iterators_[class_int] == permuter_->class_permuters_[class_int].end()) {
      iterators_[class_int] = permuter_->class_permuters_[class_int].begin();
      carry = true;
    }
    
    const std::vector<int>& class_values = *(iterators_[class_int]);
    for (unsigned int j = 0; j < class_values.size(); ++j ) {
      entries_[j].SetClass(class_int, class_values[j]);
    }
    
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

long long BruteSolutionPermuter::iterator::position() const {
  long long position = 0;
  
  for (auto it = class_types_.rbegin(); it != class_types_.rend(); ++it) {
    int class_int = *it;
    position *= permuter_->class_permuters_[class_int].permutation_count();
    position += iterators_[class_int].position();
  }
  
  return position;
}

double BruteSolutionPermuter::iterator::completion() const {
  return static_cast<double>(position()) / permuter_->permutation_count();
}

BruteSolutionPermuter::BruteSolutionPermuter(
    const EntryDescriptor* e,
    const std::vector<Solution::Cropper>& /*ignored*/)
  : entry_descriptor_(e) {
  
  const std::vector<int>& class_types =
      entry_descriptor_->AllClasses()->Values();
  
  class_permuters_.resize(class_types.size(),nullptr);
  for (auto class_int: class_types) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_[class_int] = ClassPermuter(class_descriptor);
  }
}

long long BruteSolutionPermuter::permutation_count() const {
  long long count = 1;
  for (auto permuter: class_permuters_) {
    count *= permuter.permutation_count();
  }
  return count;
}

}  // namespace Puzzle