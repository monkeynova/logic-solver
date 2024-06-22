#ifndef PUZZLE_CLASS_PERMUTER_VALUE_SKIP_TO_ACTIVE_SET_H
#define PUZZLE_CLASS_PERMUTER_VALUE_SKIP_TO_ACTIVE_SET_H

#include "absl/container/flat_hash_map.h"
#include "puzzle/active_set/active_set.h"
#include "puzzle/class_permuter/class_permuter.h"

namespace puzzle {

class ValueSkipToActiveSet {
 public:
  explicit ValueSkipToActiveSet(const ClassPermuter* class_permuter);

  const ActiveSet& value_skip_set(int position, int value) const {
    if (position >= active_set_.size()) return ActiveSet::trivial();
    if (value >= active_set_[position].size()) return ActiveSet::trivial();
    return active_set_[position][value];
  }

  const ActiveSet& value_skip_set(const ClassPermuter::iterator& it,
                                  ValueSkip value_skip) const {
    return value_skip_set(/*position=*/value_skip.value_index,
                          /*value=*/(*it)[value_skip.value_index]);
  }

 private:
  // value_index => value => active_set.
  std::vector<std::vector<ActiveSet>> active_set_;
};

}  // namespace puzzle

#endif  //  PUZZLE_CLASS_PERMUTER_VALUE_SKIP_TO_ACTIVE_SET_H
