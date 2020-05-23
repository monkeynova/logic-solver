#include "puzzle/value_skip_to_active_set.h"

namespace puzzle {

ValueSkipToActiveSet::ValueSkipToActiveSet(
    const ClassPermuter* class_permuter) {
  active_set_.resize(class_permuter->permutation_size());
  for (int i = 0; i < class_permuter->permutation_size(); ++i) {
    for (int value : class_permuter->values()) {
      active_set_[i][value] = ActiveSet();
    }
  }

  for (auto it = class_permuter->begin(ActiveSet::Empty());
       it != class_permuter->end(); ++it) {
    for (int i = 0; i < it->size(); ++i) {
      CHECK(active_set_[i].contains((*it)[i]))
          << "{" << absl::StrJoin(*it, ",") << "}; " << i;
      ActiveSet& to_add = active_set_[i][(*it)[i]];
      to_add.AddBlock(true, it.position() - to_add.total());
      to_add.Add(false);
    }
  }
  for (auto& to_finish_set : active_set_) {
    for (auto& pair : to_finish_set) {
      ActiveSet& to_finish = pair.second;
      to_finish.AddBlock(
          true, class_permuter->permutation_count() - to_finish.total());
      to_finish.DoneAdding();
    }
  }
}

}  // namespace puzzle
