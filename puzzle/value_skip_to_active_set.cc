#include "puzzle/value_skip_to_active_set.h"

namespace puzzle {

ValueSkipToActiveSet::ValueSkipToActiveSet(
    const ClassPermuter* class_permuter) {
  std::vector<absl::flat_hash_map<int, ActiveSetBuilder>> builders;

  builders.resize(class_permuter->permutation_size());
  for (int i = 0; i < class_permuter->permutation_size(); ++i) {
    for (int value : class_permuter->values()) {
      builders[i].emplace(
          value, ActiveSetBuilder(class_permuter->permutation_count()));
    }
  }

  for (auto it = class_permuter->begin(); it != class_permuter->end(); ++it) {
    for (int i = 0; i < it->size(); ++i) {
      auto to_add_it = builders[i].find((*it)[i]);
      CHECK(to_add_it != builders[i].end())
          << "{" << absl::StrJoin(*it, ",") << "}; " << i;
      ActiveSetBuilder& to_add = to_add_it->second;
      to_add.AddBlockTo(true, it.position());
      to_add.Add(false);
    }
  }

  active_set_.resize(class_permuter->permutation_size());
  for (int i = 0; i < class_permuter->permutation_size(); ++i) {
    for (int value : class_permuter->values()) {
      auto to_finish_it = builders[i].find(value);
      CHECK(to_finish_it != builders[i].end()) << i << ", " << value;
      ActiveSetBuilder& to_finish = to_finish_it->second;
      to_finish.AddBlockTo(true, class_permuter->permutation_count());
      active_set_[i].emplace(value, to_finish.DoneAdding());
    }
  }
}

}  // namespace puzzle
