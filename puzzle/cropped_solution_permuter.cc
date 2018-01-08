#include "puzzle/cropped_solution_permuter.h"

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "puzzle/active_set.h"
#include "puzzle/active_set_builder.h"

DEFINE_bool(puzzle_prune_class_iterator, true,
            "If specfied, class iterators will be pruned based on single "
            "class predicates that are present.");

DEFINE_bool(puzzle_prune_pair_class_iterators, false,
            "If specfied, class iterators will be pruned based on pair "
            "class predicates that are present.");

DEFINE_bool(puzzle_prune_reorder_classes, true,
            "If true, class iteration will be re-ordered from the default "
            "based on effective scan rate.");

namespace puzzle {

CroppedSolutionPermuter::iterator::iterator(
    const CroppedSolutionPermuter* permuter)
  : permuter_(permuter),
    mutable_solution_(permuter == nullptr
                      ? nullptr : permuter_->entry_descriptor_) {
  if (permuter_ == nullptr) return;

  current_ = mutable_solution_.TestableSolution();
  iterators_.resize(permuter_->class_permuters_.size());
  for (auto& class_permuter : permuter_->class_permuters_) {
    iterators_[class_permuter.class_int()] = class_permuter.begin();
    mutable_solution_.SetClass(iterators_[class_permuter.class_int()]);
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
    if (permuter_->profiler_ != nullptr) {
      if (permuter_->profiler_->Done()) {
        return false;
      }
      if (permuter_->profiler_->NotePosition(
              position(), permuter_->permutation_count())) {
        std::cout << "; FindNextValid(" << class_position << ") ("
                  << absl::StrJoin(
                         permuter_->class_permuters_, ", ",
                         [this](std::string* out,
                                const ClassPermuter& permuter) {
                           absl::StrAppend(
                               out,
                               iterators_[permuter.class_int()].Completion());
                         })
                  << ")" << std::flush;
      }
    }
    while(!solution_cropper.p(current_)) {
      ++iterators_[class_int];
      if (iterators_[class_int] == class_permuter.end()) {
        iterators_[class_int] = class_permuter.begin();
        mutable_solution_.SetClass(iterators_[class_int]);
        return false;
      }
      mutable_solution_.SetClass(iterators_[class_int]);
    }
    if (FindNextValid(class_position + 1)) {
      return true;
    } else {
      ++iterators_[class_int];
      mutable_solution_.SetClass(iterators_[class_int]);
    }
  }

  // Didn't find an entry in iteration. Reset iterator and return "no match".
  iterators_[class_int] = class_permuter.begin();
  mutable_solution_.SetClass(iterators_[class_int]);
  return false;
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

    mutable_solution_.SetClass(iterators_[class_int]);

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
  BuildActiveSets(croppers_with_class, &unhandled);
  ReorderEvaluation();

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
    CHECK(added)
      << "Could not add cropper for " << cropper.name << " ["
      << absl::StrJoin(cropper.classes, ",") << "]";
  }

  for (int i = 0; i < multi_class_predicates.size(); ++i) {
    std::vector<Solution::Cropper> predicates = multi_class_predicates[i];
    class_predicates_.push_back(Solution::Cropper(
        absl::StrJoin(multi_class_predicates[i], ", ",
                      [](std::string* out, const Solution::Cropper& c) {
                        absl::StrAppend(out, c.name);
                      }),
        [predicates](const Solution& s) {
          for (auto p : predicates) {
            if (!p.p(s)) return false;
          }
          return true;
        },
        {}));
  }
  if (VLOG_IS_ON(1)) {
    for (const auto& permuter : class_permuters_) {
      VLOG(1) << "Predicates at " << permuter.class_int()
              << " (" << permuter.Selectivity() << "="
              << permuter.Selectivity() * permuter.permutation_count()
              << "): " << multi_class_predicates[permuter.class_int()].size()
              << ": " << class_predicates_[permuter.class_int()].name;
    }
  }
}

void CroppedSolutionPermuter::BuildActiveSets(
    const std::vector<Solution::Cropper>& croppers,
    std::vector<Solution::Cropper>* residual) {
  if (!FLAGS_puzzle_prune_class_iterator) {
    for (const auto& cropper: croppers) {
      residual->push_back(cropper);
    }
  }

  std::vector<std::vector<Solution::Cropper>> single_class_predicates;
  std::map<std::pair<int, int>, std::vector<Solution::Cropper>>
      pair_class_predicates;
  single_class_predicates.resize(class_permuters_.size());
  for (const auto& cropper: croppers) {
    if (cropper.classes.size() == 1) {
      int class_int = cropper.classes[0];
      single_class_predicates[class_int].push_back(cropper);
    } else {
      if (cropper.classes.size() == 2) {
        std::pair<int, int> key1 = std::make_pair(
            cropper.classes[0], cropper.classes[1]);
        std::pair<int, int> key2 = std::make_pair(
            cropper.classes[0], cropper.classes[1]);
        pair_class_predicates[key1].push_back(cropper);
        pair_class_predicates[key2].push_back(cropper);
      }
      residual->push_back(cropper);
    }
  }

  ActiveSetBuilder active_set_builder(entry_descriptor_);
  for (auto& class_permuter : class_permuters_) {
    int class_int = class_permuter.class_int();
    active_set_builder.Build(class_permuter,
                             single_class_predicates[class_int]);
    VLOG(2) << "Selectivity (" << class_permuter.class_int() << "): "
            << class_permuter.Selectivity() << " => "
            << active_set_builder.active_set(class_int).Selectivity();
    class_permuter.set_active_set(active_set_builder.active_set(class_int));
  }

  if (!FLAGS_puzzle_prune_pair_class_iterators) {
    return;
  }

  bool cardinality_reduced = true;
  while (cardinality_reduced) {
    cardinality_reduced = false;

    /*
    std::sort(class_permuters_.begin(), class_permuters_.end(),
              [](const ClassPermuter& a, const ClassPermuter& b) {
                return a.Selectivity() < b.Selectivity();
                });*/
    for (auto it = class_permuters_.begin();
         it != class_permuters_.end();
         ++it) {
      for (auto it2 = it + 1; it2 != class_permuters_.end(); ++it2) {
        if (it->class_int() == it2->class_int()) continue;

        std::vector<Solution::Cropper>& croppers =
          pair_class_predicates[std::make_pair(it->class_int(),
                                               it2->class_int())];
        if (croppers.empty()) continue;

        active_set_builder.Build(*it, *it2, croppers);
        const ActiveSet& new_a = active_set_builder.active_set(it->class_int());
        const ActiveSet& new_b = active_set_builder.active_set(it2->class_int());
        VLOG(2) << "Selectivity (" << it->class_int() << ", "
                << it2->class_int() << "): (" << it->Selectivity() << ", "
                << it2->Selectivity() << ") => (" << new_a.Selectivity()
                << ", " << new_b.Selectivity() << ")";
        if (it->Selectivity() > new_a.Selectivity()) {
          cardinality_reduced = true;
        }
        if (it2->Selectivity() > new_b.Selectivity()) {
          cardinality_reduced = true;
        }
        it->set_active_set(new_a);
        it2->set_active_set(new_b);
      }
    }
  }
}

void CroppedSolutionPermuter::ReorderEvaluation() {
  if (!FLAGS_puzzle_prune_reorder_classes) {
    return;
  }

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

double CroppedSolutionPermuter::permutation_count() const {
  double count = 1;
  for (auto& permuter: class_permuters_) {
    count *= permuter.permutation_count();
  }
  return count;
}

}  // namespace puzzle
