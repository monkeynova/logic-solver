#ifndef PUZZLE_PAIR_FILTER_BURN_DOWN_H
#define PUZZLE_PAIR_FILTER_BURN_DOWN_H

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "puzzle/class_pair_selectivity.h"
#include "puzzle/class_permuter.h"
#include "puzzle/filter_to_active_set.h"
#include "thread/executor.h"

namespace puzzle {

class PairFilterBurnDown {
 public:
  PairFilterBurnDown(
      const std::vector<std::unique_ptr<ClassPermuter>>& class_permuters,
      absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>>
          pair_class_predicates,
      FilterToActiveSet* filter_to_active_set, ::thread::Executor* executor)
      : class_permuters_(class_permuters),
        pair_class_predicates_(std::move(pair_class_predicates)),
        filter_to_active_set_(filter_to_active_set),
        executor_(executor) {}

  absl::Status BurnDown();

 private:
  absl::StatusOr<std::vector<ClassPairSelectivity>> BuildSelectivityPairs();

  const std::vector<std::unique_ptr<ClassPermuter>>& class_permuters_;
  absl::flat_hash_map<std::pair<int, int>, std::vector<SolutionFilter>>
      pair_class_predicates_;
  FilterToActiveSet* filter_to_active_set_;
  ::thread::Executor* executor_;
};

};  // namespace puzzle

#endif  // PUZZLE_PAIR_FILTER_BURN_DOWN_H