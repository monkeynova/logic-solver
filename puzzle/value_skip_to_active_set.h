#ifndef PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H
#define PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H

#include "puzzle/active_set.h"
#include "puzzle/class_permuter.h"

namespace puzzle {

class ValueSkipToActiveSet {
 public:
  explicit ValueSkipToActiveSet(const ClassPermuter* class_permuter);

  const ActiveSet& value_skip_set(int position, int value) {
    return active_set_[position][value];
  }

 private:
  std::vector<std::vector<ActiveSet>> active_set_;
};

}  // namespace puzzle

#endif  //  PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H
