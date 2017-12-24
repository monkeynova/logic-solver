#include "puzzle/cropped_solution_permuter.h"

#include "gflags/gflags.h"
#include "glog/logging.h"
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
  if ((unsigned int)class_position >= permuter_->class_permuters_.size()) {
    return true;
  }

  const ClassPermuter& class_permuter =
    permuter_->class_permuters_[class_position];
  int class_int = class_permuter.class_int();

  const Solution::Cropper& solution_cropper =
    permuter_->class_predicates_[class_int];

  while (iterators_[class_int] != class_permuter.end()) {
    if (permuter_->profiler_ != nullptr && permuter_->profiler_->Done()) {
      return false;
    }
    while(!solution_cropper.p(current_)) {
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
  // TODO(keith@monkeynova.com): Allow a Solution to bind to class permuters
  // and pull values from them directly avoiding this push model.
  if (permuter_->profiler_ != nullptr) {
    if (permuter_->profiler_->NotePosition(
            position(), permuter_->permutation_count())) {
      std::cout << "; UpdateEntries(" << class_int << ") ("
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
  std::vector<int> class_order = entry_descriptor_->AllClasses()->Values();

  for (int class_int: class_order) {
    const Descriptor* class_descriptor =
        entry_descriptor_->AllClassValues(class_int);
    class_permuters_.push_back(ClassPermuter(class_descriptor, class_int));
  }

  std::vector<Solution::Cropper> unhandled;
  if (FLAGS_puzzle_prune_class_iterator) {
    std::vector<std::vector<Solution::Cropper>> single_class_predicates;
    single_class_predicates.resize(class_permuters_.size());
    for (const auto& cropper: croppers_with_class) {
      if (cropper.classes.size() == 1) {
	int class_int = cropper.classes[0];
	single_class_predicates[class_int].push_back(cropper);
      } else {
	unhandled.push_back(cropper);
      }
    }
    // Build ActiveSet for each ClassPermuter if flag enabled.
    std::vector<Entry> entries;
    Solution s = BuildSolution(&entries);
    for (auto& class_permuter : class_permuters_) {
      int class_int = class_permuter.class_int();
      class_permuter.set_active_set(BuildActiveSet(
          class_permuter, single_class_predicates[class_int],
          entry_descriptor_, &entries));
    }

    
    if (FLAGS_puzzle_prune_reorder_classes) {
      std::sort(class_permuters_.begin(), class_permuters_.end(),
		[](const ClassPermuter& a, const ClassPermuter& b) {
		  return a.Selectivity() < b.Selectivity();
		});
      VLOG(1) << "Reordered to: "
	      << absl::StrJoin(class_permuters_, ", ",
			       [](std::string* out, const ClassPermuter& a) {
				 absl::StrAppend(out, "(", a.class_int(), ",",
						 a.Selectivity(), ")");
			       });
    }
  } else {
    for (const auto& cropper: croppers_with_class) {
      unhandled.push_back(cropper);
    }
  }

  std::vector<std::vector<Solution::Cropper>> multi_class_predicates;
  multi_class_predicates.resize(class_permuters_.size());
  for (const auto& cropper: unhandled) {
    bool added = false;
    for (auto it = class_permuters_.rbegin();
	 it != class_permuters_.rend();
	 ++it) {
      int class_int = it->class_int();

      auto it2 = std::find(cropper.classes.begin(),
			   cropper.classes.end(),
			   class_int);

      if (it2 != cropper.classes.end()) {
	multi_class_predicates[class_int].push_back(cropper);
	added = true;
	break;  // class_int
      }
    }
    if (!added) {
      std::cerr << "Could not add cropper for " << cropper.name << " ["
                << absl::StrJoin(cropper.classes, ",") << "]" << std::endl;
    }
  }

  for (int i = 0; i < multi_class_predicates.size(); ++i) {
    class_predicates_.push_back(Solution::Cropper(
	absl::StrJoin(multi_class_predicates[i], ", ",
		      [](std::string* out, const Solution::Cropper& c) {
			absl::StrAppend(out, c.name);
		      }),
	[multi_class_predicates, i](const Solution& s) {
	  for (auto p : multi_class_predicates[i]) {
	    if (!p.p(s)) return false;
	  }
	  return true;
	},
	{}));
    VLOG(1) << "Predicates at " << i << " (" << class_permuters_[i].Selectivity()
	    << "): " << multi_class_predicates[i].size()
	    << ": " << class_predicates_[i].name;
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
