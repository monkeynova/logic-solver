#ifndef PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H
#define PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H

#include "absl/container/flat_hash_map.h"
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
  // TODO(keith@monkeynova.com): flat_hash_map allows IntRangeDescriptor to
  // return non-0-indexed values, but costs about 2x the CPU to construct
  // these ActiveSets on the benchmark.
 std::vector<absl::flat_hash_map<int, ActiveSet>> active_set_;
};

}  // namespace puzzle

#endif  //  PUZZLE_VALUE_SKIP_TO_ACTIVE_SET_H
