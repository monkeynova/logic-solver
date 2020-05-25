#ifndef PUZZLE_ACTIVE_SET_PAIR_H
#define PUZZLE_ACTIVE_SET_PAIR_H

#include "absl/container/flat_hash_map.h"
#include "puzzle/active_set.h"

namespace puzzle {

class ActiveSetPair {
 public:
  ActiveSetPair() = default;

  const ActiveSet& Find(int a_val) const {
    auto it = b_given_a_.find(a_val);
    if (it == b_given_a_.end()) return ActiveSet::trivial();
    return it->second;
  }

  void Assign(int a_val, ActiveSet a_b_set) {
    b_given_a_[a_val] = std::move(a_b_set);
  }

 private:
  absl::flat_hash_map<int, ActiveSet> b_given_a_;
};

}  // namespace puzzle

#endif  //  PUZZLE_ACTIVE_SET_PAIR_H
