#include "puzzle/active_set_builder.h"

namespace puzzle {

ActiveSet ActiveSetBuilder::Build(
    const ClassPermuter& class_permuter,
    const std::vector<Solution::Cropper>& predicates) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 1);
    CHECK_EQ(p.classes[0], class_permuter.class_int());
  }
  ActiveSet source = class_permuter.active_set();
  ActiveSet active_set;
  Solution s = mutable_solution_.TestableSolution();
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    mutable_solution_.SetClass(it);
    active_set.AddFalseBlock(source.ConsumeFalseBlock());
    CHECK(source.ConsumeNext());
    active_set.Add(std::all_of(predicates.begin(),
			       predicates.end(),
			       [&s](const Solution::Cropper& c) {
				 return c.p(s);
			       }));
  }
  active_set.AddFalseBlock(source.ConsumeFalseBlock());
  CHECK(source.ConsumeNext());
  active_set.DoneAdding();
  return active_set;
}

void ActiveSetBuilder::Build(
    const ClassPermuter& class_permuter_a,
    const ClassPermuter& class_permuter_b,
    const std::vector<Solution::Cropper>& predicates,
    ActiveSet* active_set_a,
    ActiveSet* active_set_b) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 2);
    CHECK(p.classes[0] == class_permuter_a.class_int() ||
	  p.classes[0] == class_permuter_b.class_int());
    CHECK(p.classes[1] == class_permuter_a.class_int() ||
	  p.classes[1] == class_permuter_b.class_int());
    CHECK_NE(p.classes[0], p.classes[1]);
  }
  *active_set_a = ActiveSet();
  ActiveSet source_a = class_permuter_a.active_set();
  std::set<int> b_match_positions;
  Solution s = mutable_solution_.TestableSolution();
  double full_match_count = 0;
  for (auto it_a = class_permuter_a.begin();
       it_a != class_permuter_a.end();
       ++it_a) {
    mutable_solution_.SetClass(it_a);
    active_set_a->AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    bool any_of_a = false;
    for (auto it_b = class_permuter_b.begin();
	 it_b != class_permuter_b.end();
	 ++it_b) {
      if (any_of_a &&
	  b_match_positions.find(it_b.position()) != b_match_positions.end()) {
	// Already added both pieces.
	continue;
      }
      mutable_solution_.SetClass(it_b);
      const bool this_match = std::all_of(predicates.begin(),
					  predicates.end(),
					  [&s](const Solution::Cropper& c) {
					    return c.p(s);
					  });
      if (this_match) {
	++full_match_count;
	any_of_a = true;
	b_match_positions.insert(it_b.position());
      }
    }
    active_set_a->Add(any_of_a);
  }
  active_set_a->AddFalseBlock(source_a.ConsumeFalseBlock());
  CHECK(source_a.ConsumeNext());
  active_set_a->DoneAdding();

  *active_set_b = ActiveSet(b_match_positions,
			    class_permuter_b.permutation_count());

  VLOG(1) << "Join selectivity (" << class_permuter_a.class_int() << ", "
	  << class_permuter_b.class_int() << ")="
	  << full_match_count / (active_set_a->matches() *
				 active_set_b->matches())
	  << ": " << full_match_count << " out of "
	  << active_set_a->matches() * active_set_b->matches();
}

}  // namespace puzzle
