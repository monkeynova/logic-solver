#include "puzzle/active_set_builder.h"

namespace puzzle {

ActiveSet ActiveSetBuilder::Build(
    const ClassPermuter& class_permuter,
    const std::vector<Solution::Cropper>& predicates) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 1);
    CHECK_EQ(p.classes[0], class_permuter.class_int());
  }
  ActiveSet active_set;
  Solution s = mutable_solution_.TestableSolution();
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    mutable_solution_.SetClass(it);
    active_set.Add(std::all_of(predicates.begin(),
			       predicates.end(),
			       [&s](const Solution::Cropper& c) {
				 return c.p(s);
			       }));
  }
  active_set.DoneAdding();
  return active_set;
}

}  // namespace puzzle
