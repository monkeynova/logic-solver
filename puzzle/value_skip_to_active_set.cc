#include "puzzle/value_skip_to_active_set.h"

namespace puzzle {

ValueSkipToActiveSet::ValueSkipToActiveSet(const ClassPermuter* class_permuter)
    : active_set_(class_permuter->permutation_size(),
                  std::vector<ActiveSet>(class_permuter->permutation_size())) {
  for (auto it = class_permuter->begin(); it != class_permuter->end(); ++it) {
    for (int i = 0; i < it->size(); ++i) {
      ActiveSet& to_add = active_set_[i][(*it)[i]];
      to_add.AddBlock(true, it.position() - to_add.total());
      to_add.Add(false);
    }
  }
  for (auto& to_finish_set : active_set_) {
    for (auto& to_finish : to_finish_set) {
      to_finish.AddBlock(
          true, class_permuter->permutation_count() - to_finish.total());
      to_finish.DoneAdding();
    }
  }
}

}  // namespace puzzle
