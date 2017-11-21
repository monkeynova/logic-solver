#include "puzzle/cropped_solution_permuter.h"

namespace Puzzle {

CroppedSolutionPermuter::iterator::iterator(
    const CroppedSolutionPermuter* permuter, 
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
  for (int class_int: class_types_) {
    iterators_[class_int] = permuter_->class_permuters_[class_int].begin();
    
    UpdateEntries(class_int);
  }
  
  current_ = Solution(&entries_);
  
  if (FindNextValid(0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

//#define DEBUG_CROP

bool CroppedSolutionPermuter::iterator::FindNextValid(int class_position) {
  if ((unsigned int)class_position >= class_types_.size()) {
    return true;
  }
  
  int class_int = class_types_[class_position];
  int found = false;

  const ClassPermuter& class_permuter = permuter_->class_permuters_[class_int];
  const std::vector<SolutionCropper>& solution_cropper =
      permuter_->class_crop_predicates_[class_int];

  while (!found && iterators_[class_int] != class_permuter.end()) {
    while(!std::all_of(solution_cropper.begin(),
		       solution_cropper.end(),
		       [this](const SolutionCropper& c) {
			 return c.p_(current_);
		       })) {
      ++iterators_[class_int];
      if (iterators_[class_int] == class_permuter.end()) {
	iterators_[class_int] = class_permuter.begin();
                return false;
      }
      UpdateEntries(class_int);
    }
    if (FindNextValid(class_position+1)) {
      found = true;
    } else {
      ++iterators_[class_int];
      UpdateEntries(class_int);
    }
  }
  
  return found;
}

void CroppedSolutionPermuter::iterator::UpdateEntries(int class_int) {
  const std::vector<int>& class_values = *(iterators_[class_int]);
  for (unsigned int j = 0; j < class_values.size(); ++j ) {
    entries_[j].SetClass(class_int, class_values[j]);
  }
}

void CroppedSolutionPermuter::iterator::Advance() {
  bool at_end = true;
  for (int class_int: class_types_) {
    ++iterators_[class_int];
    
    bool carry = false;
    if (iterators_[class_int] == permuter_->class_permuters_[class_int].end()) {
      iterators_[class_int] = permuter_->class_permuters_[class_int].begin();
      carry = true;
    }
    
    UpdateEntries(class_int);
    
    if (!carry) {
      at_end = false;
      break;
    }
  }
  if (!at_end && FindNextValid(0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

long long CroppedSolutionPermuter::iterator::position() const {
  long long position = 0;
  
  for (auto it = class_types_.rbegin(); it != class_types_.rend(); ++it) {
    int class_int = *it;
    position *= permuter_->class_permuters_[class_int].permutation_count();
    position += iterators_[class_int].position();
  }
  
  return position;
}

double CroppedSolutionPermuter::iterator::completion() const {
  return static_cast<double>(position()) / permuter_->permutation_count();
}

CroppedSolutionPermuter::CroppedSolutionPermuter(
    const EntryDescriptor* e, 
    const std::vector<SolutionCropper>& croppers_with_class)
   : entry_descriptor_(e) { 
  const std::vector<int>& class_types =
      entry_descriptor_->AllClasses()->Values();
  
  class_permuters_.resize(class_types.size(),nullptr);
  for (int class_int: class_types) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_[class_int] = ClassPermuter(class_descriptor);
  }
  
  class_crop_predicates_.resize(class_types.size());
  for (auto cropper: croppers_with_class) {
    for (auto it = class_types.rbegin(); it != class_types.rend(); ++it) {
      int class_int = *it;
      
      auto it2 = std::find_if(cropper.classes_.begin(),
			      cropper.classes_.end(),
			      [class_int](int find_int) {
				return class_int == find_int;
			      });
      if (it2 != cropper.classes_.end()) {
	class_crop_predicates_[class_int].push_back(cropper);
	break;  // class_int
      }
    }
  }
}

long long CroppedSolutionPermuter::permutation_count() const {
  long long count = 1;
  for (auto permuter: class_permuters_) {
    count *= permuter.permutation_count();
  }
  return count;
}

}  // namespace Puzzle
