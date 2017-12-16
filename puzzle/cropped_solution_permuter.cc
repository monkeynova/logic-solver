#include "puzzle/cropped_solution_permuter.h"

#include "gflags/gflags.h"
#include "puzzle/active_set.h"

DEFINE_bool(puzzle_prune_class_iterator, false,
	    "If specfied, class iterators will be pruned based on single "
	    "class predicates that are present.");

namespace puzzle {

CroppedSolutionPermuter::iterator::iterator(
    const CroppedSolutionPermuter* permuter)
   : permuter_(permuter) {
  if (permuter_ == nullptr) return;
  
  current_ = permuter_->BuildSolution(&entries_);

  class_types_ = current_.descriptor()->AllClasses()->Values();

  iterators_.resize(class_types_.size());
  for (int class_int: class_types_) {
    iterators_[class_int] = permuter_->class_permuters_[class_int].begin();
    UpdateEntries(class_int);
  }

  active_sets_.resize(class_types_.size());
  if (FLAGS_puzzle_prune_class_iterator) {
    for (int class_int: class_types_) {
      PruneClass(class_int, permuter_->single_class_predicates_[class_int]);
    }
  }

  if (FindNextValid(0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

void CroppedSolutionPermuter::iterator::PruneClass(
    int class_int, const std::vector<Solution::Cropper>& predicates) {
  ActiveSet active_set;
  const ClassPermuter& class_permuter = permuter_->class_permuters_[class_int];
  for (iterators_[class_int] = class_permuter.begin();
       iterators_[class_int] != class_permuter.end();
       ++iterators_[class_int]) {
    UpdateEntries(class_int);
    active_set.AddSkip(std::all_of(predicates.begin(),
				   predicates.end(),
				   [this](const Solution::Cropper& c) {
				     return c.p(current_);
				   }));
  }
  active_set.DoneAdding();
  std::cout << class_int << " => " << active_set.Selectivity() << std::endl;
  active_sets_[class_int] = std::move(active_set);
  iterators_[class_int] = class_permuter.begin(active_sets_[class_int]);
  UpdateEntries(class_int);
}

bool CroppedSolutionPermuter::iterator::FindNextValid(int class_position) {
  if ((unsigned int)class_position >= class_types_.size()) {
    return true;
  }
  
  int class_int = class_types_[class_position];

  const ClassPermuter& class_permuter = permuter_->class_permuters_[class_int];
  const std::vector<Solution::Cropper>& solution_cropper =
      permuter_->multi_class_predicates_[class_int];

  while (iterators_[class_int] != class_permuter.end()) {
    if (permuter_->profiler_ != nullptr && permuter_->profiler_->Done()) {
      return false;
    }
    while(!std::all_of(solution_cropper.begin(),
		       solution_cropper.end(),
		       [this](const Solution::Cropper& c) {
			 return c.p(current_);
		       })) {
      ++iterators_[class_int];
      if (iterators_[class_int] == class_permuter.end()) {
	iterators_[class_int] = class_permuter.begin(active_sets_[class_int]);
        UpdateEntries(class_int);
        return false;
      }
      UpdateEntries(class_int);
    }
    if (FindNextValid(class_position+1)) {
      return true;
    } else {
      ++iterators_[class_int];
      UpdateEntries(class_int);
    }
  }

  // Didn't find an entry in iteration. Reset iterator and return "no match".
  iterators_[class_int] = class_permuter.begin(active_sets_[class_int]);
  UpdateEntries(class_int);
  return false;
}

void CroppedSolutionPermuter::iterator::UpdateEntries(int class_int) {
  if (permuter_->profiler_ != nullptr) {
    if (permuter_->profiler_->NotePosition(
	    position(), permuter_->permutation_count())) {
      std::cout << "\033[1K\rUpdateEntries(" << class_int << ") ("
		<< absl::StrJoin(iterators_, ", ",
				 [](std::string* out,
				    const ClassPermuter::iterator& it) {
				   absl::StrAppend(out, it.completion());
				 })
		<< "): " << position() << std::flush;
    }
  }
  const std::vector<int>& class_values = *(iterators_[class_int]);
  for (unsigned int j = 0; j < class_values.size(); ++j ) {
    entries_[j].SetClass(class_int, class_values[j]);
  }
}

void CroppedSolutionPermuter::iterator::Advance() {
  bool at_end = true;
  for (auto it = class_types_.rbegin(); it != class_types_.rend(); ++it) {
    int class_int = *it;
    ++iterators_[class_int];
    
    bool carry = false;
    if (iterators_[class_int] == permuter_->class_permuters_[class_int].end()) {
      iterators_[class_int] =
	  permuter_->class_permuters_[class_int].begin(active_sets_[class_int]);
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

double CroppedSolutionPermuter::iterator::position() const {
  double position = 0;

  for (int class_int : class_types_) {
    position *= permuter_->class_permuters_[class_int].permutation_count();
    position += iterators_[class_int].position();
  }
  
  return position;
}

double CroppedSolutionPermuter::iterator::completion() const {
  return position() / permuter_->permutation_count();
}

CroppedSolutionPermuter::CroppedSolutionPermuter(
    const EntryDescriptor* e, 
    const std::vector<Solution::Cropper>& croppers_with_class,
    Profiler* profiler)
    : entry_descriptor_(e), profiler_(profiler) { 
  const std::vector<int>& class_types =
      entry_descriptor_->AllClasses()->Values();
  
  single_class_predicates_.resize(class_types.size());
  multi_class_predicates_.resize(class_types.size());
  for (auto cropper: croppers_with_class) {
    bool added = false;
    if (cropper.classes.size() == 1) {
      int class_int = cropper.classes[0];
      single_class_predicates_[class_int].push_back(cropper);      
      added = true;
    } else {
      for (auto it = class_types.rbegin(); it != class_types.rend(); ++it) {
	int class_int = *it;

	auto it2 = std::find(cropper.classes.begin(),
			     cropper.classes.end(),
			     class_int);

	if (it2 != cropper.classes.end()) {
	  multi_class_predicates_[class_int].push_back(cropper);
	  added = true;
	  break;  // class_int
	}
      }
    }
    if (!added) {
      std::cerr << "Could not add cropper for " << cropper.name << " ["
                << absl::StrJoin(cropper.classes, ",") << "]" << std::endl;
    }
  }

  if (!FLAGS_puzzle_prune_class_iterator) {
    for (int i = 0; i < single_class_predicates_.size(); ++i) {
      for (auto p : single_class_predicates_[i]) {
	multi_class_predicates_[i].emplace_back(p);
      }
    }
  }

  class_permuters_.resize(class_types.size(), nullptr);
  for (int class_int: class_types) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_[class_int] = ClassPermuter(class_descriptor);
  }
}

double CroppedSolutionPermuter::permutation_count() const {
  double count = 1;
  for (auto permuter: class_permuters_) {
    count *= permuter.permutation_count();
  }
  return count;
}

Solution CroppedSolutionPermuter::BuildSolution(
    std::vector<Entry>* entries) const {
  const int num_classes = entry_descriptor_->AllClasses()->Values().size();
  
  std::vector<int> invalid_classes(num_classes, -1);
    
  for (auto id: entry_descriptor_->AllIds()->Values()) {
    entries->push_back(Entry(id, invalid_classes, entry_descriptor_));
  }

  return Solution(entry_descriptor_, entries);
}

}  // namespace puzzle
