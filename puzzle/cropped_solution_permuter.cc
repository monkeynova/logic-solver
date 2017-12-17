#include "puzzle/cropped_solution_permuter.h"

#include "gflags/gflags.h"
#include "puzzle/active_set.h"

DEFINE_bool(puzzle_prune_class_iterator, false,
            "If specfied, class iterators will be pruned based on single "
            "class predicates that are present.");

DEFINE_bool(puzzle_prune_reorder_classes, false,
	    "If true, class iteration will be re-ordered from the default "
	    "based on effective scan rate.");

namespace puzzle {

static void SetClassFromPermutation(
    const ClassPermuter::iterator& it,
    std::vector<Entry>* entries) {
  for (unsigned int j = 0; j < it->size(); ++j ) {
    (*entries)[j].SetClass(it.class_int(), (*it)[j]);
  }
}

static ActiveSet BuildActiveSet(
    const ClassPermuter& class_permuter,
    const std::vector<Solution::Cropper>& predicates,
    const EntryDescriptor* entry_descriptor,
    std::vector<Entry>* entries) {
  Solution s(entry_descriptor, entries);
  ActiveSet active_set;
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    SetClassFromPermutation(it, entries);
    active_set.AddSkip(std::all_of(predicates.begin(),
                                   predicates.end(),
                                   [&s](const Solution::Cropper& c) {
                                     return c.p(s);
                                   }));
  }
  active_set.DoneAdding();
  std::cout << class_permuter.class_int() << " => "
            << active_set.Selectivity() << std::endl;
  return active_set;
}

CroppedSolutionPermuter::iterator::iterator(
    const CroppedSolutionPermuter* permuter)
   : permuter_(permuter) {
  if (permuter_ == nullptr) return;
  
  current_ = permuter_->BuildSolution(&entries_);
  iterators_.resize(permuter_->class_permuters_.size());
  for (auto& class_permuter : permuter_->class_permuters_) {
    iterators_[class_permuter.class_int()] = class_permuter.begin();
  }
  // UpdateEntries requires all iterators to be constructed.
  for (auto& class_permuter : permuter_->class_permuters_) {
    UpdateEntries(class_permuter.class_int());
  }
  
  if (FindNextValid(/*class_position=*/0)) {
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(position());
  } else {
    current_ = Solution();
    current_.set_permutation_count(permuter_->permutation_count());
    current_.set_permutation_position(permuter_->permutation_count());
  }
}

bool CroppedSolutionPermuter::iterator::FindNextValid(int class_position) {
  if ((unsigned int)class_position >= permuter_->class_order().size()) {
    return true;
  }

  int class_int = permuter_->class_order()[class_position];

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
        iterators_[class_int] = class_permuter.begin();
        UpdateEntries(class_int);
        return false;
      }
      UpdateEntries(class_int);
    }
    if (FindNextValid(class_position + 1)) {
      return true;
    } else {
      ++iterators_[class_int];
      UpdateEntries(class_int);
    }
  }

  // Didn't find an entry in iteration. Reset iterator and return "no match".
  iterators_[class_int] = class_permuter.begin();
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
                                   absl::StrAppend(out, it.Completion());
                                 })
                << "): " << position() << std::flush;
    }
  }
  SetClassFromPermutation(iterators_[class_int], &entries_);
}

void CroppedSolutionPermuter::iterator::Advance() {
  bool at_end = true;
  for (auto it = permuter_->class_permuters_.rbegin();
       it != permuter_->class_permuters_.rend();
       ++it) {
    int class_int = it->class_int();
    ++iterators_[class_int];
    
    bool carry = false;
    if (iterators_[class_int] == it->end()) {
      iterators_[class_int] = it->begin();
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

  for (auto& class_permuter : permuter_->class_permuters_) {
    position *= class_permuter.permutation_count();
    position += iterators_[class_permuter.class_int()].position();
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
  class_order_ = entry_descriptor_->AllClasses()->Values();
  if (FLAGS_puzzle_prune_reorder_classes) {
    std::reverse(class_order_.begin(), class_order_.end());
    std::cout << absl::StrJoin(class_order_, ", ") << std::endl;
  }

  std::vector<std::vector<Solution::Cropper>> single_class_predicates;
  single_class_predicates.resize(class_order_.size());

  multi_class_predicates_.resize(class_order_.size());
  for (auto cropper: croppers_with_class) {
    bool added = false;
    if (cropper.classes.size() == 1) {
      int class_int = cropper.classes[0];
      single_class_predicates[class_int].push_back(cropper);
      added = true;
    } else {
      for (auto it = class_order_.rbegin(); it != class_order_.rend(); ++it) {
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

  class_permuters_.resize(class_order_.size());
  for (int class_int: class_order_) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_[class_int] = ClassPermuter(class_descriptor, class_int);
  }

  if (FLAGS_puzzle_prune_class_iterator) {
    // Build ActiveSet for each ClassPermuter if flag enabled.
    std::vector<Entry> entries;
    Solution s = BuildSolution(&entries);
    for (int class_int : class_order_) {
      class_permuters_[class_int].set_active_set(BuildActiveSet(
          class_permuters_[class_int], single_class_predicates[class_int],
          entry_descriptor_, &entries));
    }
  } else {
    // Otherwise add single class filters to the rest of the filters.
    for (int i = 0; i < single_class_predicates.size(); ++i) {
      for (auto p : single_class_predicates[i]) {
        multi_class_predicates_[i].emplace_back(p);
      }
    }
  }
}

double CroppedSolutionPermuter::permutation_count() const {
  double count = 1;
  for (auto& permuter: class_permuters_) {
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
