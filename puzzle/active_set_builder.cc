#include "puzzle/active_set_builder.h"

namespace puzzle {

template <>
void ActiveSetBuilder::Build<ActiveSetBuilder::SingleClassBuild::kPassThrough>(
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
  active_sets_[class_permuter.class_int()] = active_set;
}

template <>
void ActiveSetBuilder::Build<ActiveSetBuilder::SingleClassBuild::kPositionSet>(
    const ClassPermuter& class_permuter,
    const std::vector<Solution::Cropper>& predicates) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 1);
    CHECK_EQ(p.classes[0], class_permuter.class_int());
  }
  std::set<int> a_matches;
  Solution s = mutable_solution_.TestableSolution();
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    mutable_solution_.SetClass(it);
    if (std::all_of(predicates.begin(),
		    predicates.end(),
		    [&s](const Solution::Cropper& c) {
		      return c.p(s);
		    })) {
      a_matches.insert(it.position());
    }
  }
  active_sets_[class_permuter.class_int()] = ActiveSet(
      a_matches, class_permuter.permutation_count());
}

void ActiveSetBuilder::Build(
    const ClassPermuter& class_permuter_a,
    const ClassPermuter& class_permuter_b,
    const std::vector<Solution::Cropper>& predicates) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 2);
    CHECK(p.classes[0] == class_permuter_a.class_int() ||
          p.classes[0] == class_permuter_b.class_int());
    CHECK(p.classes[1] == class_permuter_a.class_int() ||
          p.classes[1] == class_permuter_b.class_int());
    CHECK_NE(p.classes[0], p.classes[1]);
  }
  Solution s = mutable_solution_.TestableSolution();
  ActiveSet active_set_a = ActiveSet();
  ActiveSet active_set_b = ActiveSet();
  {
    ActiveSet source_a = class_permuter_a.active_set();
    for (auto it_a = class_permuter_a.begin();
         it_a != class_permuter_a.end();
         ++it_a) {
      mutable_solution_.SetClass(it_a);
      active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
      CHECK(source_a.ConsumeNext());
      bool any_of_a = false;
      for (auto it_b = class_permuter_b.begin();
           it_b != class_permuter_b.end();
           ++it_b) {
        mutable_solution_.SetClass(it_b);
        const bool this_match = std::all_of(predicates.begin(),
                                            predicates.end(),
                                            [&s](const Solution::Cropper& c) {
                                              return c.p(s);
                                            });
        if (this_match) {
          any_of_a = true;
          break;
        }
      }
      active_set_a.Add(any_of_a);
    }
    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    active_set_a.DoneAdding();
  }
  {
    ActiveSet source_b = class_permuter_b.active_set();
    for (auto it_b = class_permuter_b.begin();
         it_b != class_permuter_b.end();
         ++it_b) {
      mutable_solution_.SetClass(it_b);
      active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
      CHECK(source_b.ConsumeNext());
      bool any_of_b = false;
      for (auto it_a = class_permuter_a.begin();
           it_a != class_permuter_a.end();
           ++it_a) {
        mutable_solution_.SetClass(it_a);
        const bool this_match = std::all_of(predicates.begin(),
                                            predicates.end(),
                                            [&s](const Solution::Cropper& c) {
                                              return c.p(s);
                                            });
        if (this_match) {
          any_of_b = true;
          break;
        }
      }
      active_set_b.Add(any_of_b);
    }
    active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
    CHECK(source_b.ConsumeNext());
    active_set_b.DoneAdding();
  }

  active_sets_[class_permuter_a.class_int()] = std::move(active_set_a);
  active_sets_[class_permuter_b.class_int()] = std::move(active_set_b);
}

void ActiveSetBuilder::Build(
    SingleClassBuild single_class_build,
    const ClassPermuter& class_permuter,
    const std::vector<Solution::Cropper>& predicates) {
  switch (single_class_build) {
  case SingleClassBuild::kPassThrough:
    Build<SingleClassBuild::kPassThrough>(class_permuter, predicates);
    return;
  case SingleClassBuild::kPositionSet:
    Build<SingleClassBuild::kPositionSet>(class_permuter, predicates);
    return;
  default:
    LOG(FATAL) << "Bad SingleClassBuild "
      << static_cast<int>(single_class_build);
  }
}

}  // namespace puzzle
