#ifndef PUZZLE_PAIR_FILTER_BURN_DOWN_H
#define PUZZLE_PAIR_FILTER_BURN_DOWN_H

#include "absl/status/status.h"
#include "puzzle/class_permuter.h"
#include "puzzle/filter_to_active_set.h"
#include "thread/executor.h"

namespace puzzle {

class PairFilterBurnDown {
 public:
  PairFilterBurnDown(
      const std::vector<std::unique_ptr<ClassPermuter>>& class_permuters,
      FilterToActiveSet* filter_to_active_set, ::thread::Executor* executor)
      : class_permuters_(class_permuters),
        filter_to_active_set_(filter_to_active_set),
        executor_(executor) {}

  absl::Status BurnDown(
      absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>>
          pair_class_predicates);

 private:
  const std::vector<std::unique_ptr<ClassPermuter>>& class_permuters_;
  FilterToActiveSet* filter_to_active_set_;
  ::thread::Executor* executor_;
};

};  // namespace puzzle

#endif  // PUZZLE_PAIR_FILTER_BURN_DOWN_H