#ifndef PUZZLE_ACTIVE_SET_PAIR_H
#define PUZZLE_ACTIVE_SET_PAIR_H

#include "absl/container/flat_hash_map.h"
#include "puzzle/active_set.h"

namespace puzzle {

class ActiveSetPair {
 public:
  ActiveSetPair() = default;

  const ActiveSet& Find(int a_val) const {
    auto it = map_.find(a_val);
    if (it == map_.end()) {
      static ActiveSet empty;
      return empty;
    }
    return it->second;
  }

  void Assign(int a_val, ActiveSet a_b_set) {
    map_[a_val] = std::move(a_b_set);
  }
  
  void Clear() { map_.clear(); }
  
 private:
  absl::flat_hash_map<int, ActiveSet> map_;
};
  
}  // namespace puzzle

#endif  //  PUZZLE_ACTIVE_SET_PAIR_H
