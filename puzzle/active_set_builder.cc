#include "puzzle/active_set_builder.h"

namespace puzzle {

ActiveSetBuilder::ActiveSetBuilder(const EntryDescriptor* entry_descriptor)
  : active_sets_(entry_descriptor == nullptr
		 ? 0 : entry_descriptor->num_classes()),
    mutable_solution_(entry_descriptor) {
  if (entry_descriptor != nullptr) {
    int num_classes = entry_descriptor->num_classes();
    active_set_pairs_.resize(num_classes);
    for (int i = 0; i < num_classes; ++i) {
      active_set_pairs_[i].resize(num_classes);
    }
  }
  solution_ = mutable_solution_.TestableSolution();
}

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
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    mutable_solution_.SetClass(it);
    active_set.AddFalseBlock(source.ConsumeFalseBlock());
    CHECK(source.ConsumeNext());
    active_set.Add(std::all_of(predicates.begin(),
                               predicates.end(),
                               [this](const Solution::Cropper& c) {
                                 return c.p(solution_);
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
  for (auto it = class_permuter.begin();
       it != class_permuter.end();
       ++it) {
    mutable_solution_.SetClass(it);
    if (std::all_of(predicates.begin(),
                    predicates.end(),
                    [this](const Solution::Cropper& c) {
                      return c.p(solution_);
                    })) {
      a_matches.insert(it.position());
    }
  }
  active_sets_[class_permuter.class_int()] = ActiveSet(
      a_matches, class_permuter.permutation_count());
}

void ActiveSetBuilder::SetupPairBuild(
    int class_a, int class_b,
    const std::vector<Solution::Cropper>& predicates) {
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes.size(), 2);
    CHECK(p.classes[0] == class_a || p.classes[0] == class_b);
    CHECK(p.classes[1] == class_a || p.classes[1] == class_b);
    CHECK_NE(p.classes[0], p.classes[1]);
  }
  active_set_pairs_[class_a][class_b].clear();
  active_set_pairs_[class_b][class_a].clear();
}
  

template <>
void ActiveSetBuilder::Build<ActiveSetBuilder::PairClassImpl::kBackAndForth>(
    const ClassPermuter& permuter_a,
    const ClassPermuter& permuter_b,
    const std::vector<Solution::Cropper>& predicates,
    ActiveSetBuilder::PairClassMode pair_class_mode) { 
  int class_a = permuter_a.class_int();
  int class_b = permuter_b.class_int();
  SetupPairBuild(class_a, class_b, predicates);
  {
    ActiveSet active_set_a = ActiveSet();
    ActiveSet source_a = permuter_a.active_set();

    for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
      mutable_solution_.SetClass(it_a);
      active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
      CHECK(source_a.ConsumeNext());
      bool any_of_a = false;
      ActiveSet a_b_set;
      ActiveSet source_b = permuter_b.active_set();
      for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
        mutable_solution_.SetClass(it_b);
        const bool this_match = std::all_of(
            predicates.begin(),
            predicates.end(),
            [this](const Solution::Cropper& c) {
              return c.p(solution_);
            });
        if (this_match) {
          any_of_a = true;
          if (pair_class_mode == PairClassMode::kSingleton) break;
        }
        if (pair_class_mode == PairClassMode::kMakePairs) {
          a_b_set.AddFalseBlock(source_b.ConsumeFalseBlock());
          CHECK(source_b.ConsumeNext());
          a_b_set.Add(this_match);
        }
      }
      active_set_a.Add(any_of_a);
      if (pair_class_mode == PairClassMode::kMakePairs && any_of_a) {
        a_b_set.DoneAdding();
        active_set_pairs_[class_a][class_b][it_a.position()] =
          std::move(a_b_set);
      }
    }

    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    active_set_a.DoneAdding();
    active_sets_[class_a] = std::move(active_set_a);
  }
  {
    ActiveSet active_set_b = ActiveSet();
    ActiveSet source_b = permuter_b.active_set();

    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution_.SetClass(it_b);
      active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
      CHECK(source_b.ConsumeNext());
      ActiveSet b_a_set;
      ActiveSet source_a = permuter_a.active_set();
      bool any_of_b = false;
      for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
        mutable_solution_.SetClass(it_a);
        const bool this_match = std::all_of(
            predicates.begin(),
            predicates.end(),
            [this](const Solution::Cropper& c) {
              return c.p(solution_);
            });
        if (this_match) {
          any_of_b = true;
          if (pair_class_mode == PairClassMode::kSingleton) break;
        }
        if (pair_class_mode == PairClassMode::kMakePairs) {
          b_a_set.AddFalseBlock(source_a.ConsumeFalseBlock());
          CHECK(source_a.ConsumeNext());
          b_a_set.Add(this_match);
        }
      }
      active_set_b.Add(any_of_b);
      if (pair_class_mode == PairClassMode::kMakePairs && any_of_b) {
        b_a_set.DoneAdding();
        active_set_pairs_[class_b][class_a][it_b.position()] =
          std::move(b_a_set);
      }
    }

    active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
    CHECK(source_b.ConsumeNext());
    active_set_b.DoneAdding();
    active_sets_[class_b] = std::move(active_set_b);
  }
}

template <>
void ActiveSetBuilder::Build<ActiveSetBuilder::PairClassImpl::kPassThroughA>(
    const ClassPermuter& permuter_a,
    const ClassPermuter& permuter_b,
    const std::vector<Solution::Cropper>& predicates,
    ActiveSetBuilder::PairClassMode pair_class_mode) {
  int class_a = permuter_a.class_int();
  int class_b = permuter_b.class_int();
  SetupPairBuild(class_a, class_b, predicates);
  ActiveSet active_set_a = ActiveSet();
  ActiveSet source_a = permuter_a.active_set();
  std::set<int> b_match_positions;
  std::map<int, std::set<int>> b_a_match_positions;

  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution_.SetClass(it_a);
    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    bool any_of_a = false;
    ActiveSet a_b_set;
    ActiveSet source_b = permuter_b.active_set();
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      if (pair_class_mode == PairClassMode::kSingleton && any_of_a &&
          b_match_positions.find(it_b.position()) != b_match_positions.end()) {
        // Already added both pieces.
        continue;
      }
      mutable_solution_.SetClass(it_b);
      const bool this_match = std::all_of(
          predicates.begin(),
          predicates.end(),
          [this](const Solution::Cropper& c) {
            return c.p(solution_);
          });
      if (this_match) {
        any_of_a = true;
        b_match_positions.insert(it_b.position());
      }
      if (pair_class_mode == PairClassMode::kMakePairs) {
        a_b_set.AddFalseBlock(source_b.ConsumeFalseBlock());
        CHECK(source_b.ConsumeNext());
        a_b_set.Add(this_match);
        if (this_match) {
          b_a_match_positions[it_b.position()].insert(it_a.position());
        }
      }
    }
    if (pair_class_mode == PairClassMode::kMakePairs && any_of_a) {
      a_b_set.DoneAdding();
      active_set_pairs_[class_a][class_b][it_a.position()] =
        std::move(a_b_set);
    }
    active_set_a.Add(any_of_a);
  }

  active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
  CHECK(source_a.ConsumeNext());
  active_set_a.DoneAdding();
  active_sets_[class_a] = std::move(active_set_a);
  active_sets_[class_b] = ActiveSet(
      b_match_positions, permuter_b.permutation_count());
  if (pair_class_mode == PairClassMode::kMakePairs) {
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const std::set<int>& a_set = pair.second;
      active_set_pairs_[class_b][class_a][b_val] = ActiveSet(
          a_set, permuter_a.permutation_count());
    }
  }
}

template <>
void ActiveSetBuilder::Build<ActiveSetBuilder::PairClassImpl::kPairSet>(
    const ClassPermuter& permuter_a,
    const ClassPermuter& permuter_b,
    const std::vector<Solution::Cropper>& predicates,
    ActiveSetBuilder::PairClassMode pair_class_mode) {
  int class_a = permuter_a.class_int();
  int class_b = permuter_b.class_int();
  SetupPairBuild(class_a, class_b, predicates);
  Solution solution = mutable_solution_.TestableSolution();
  std::set<int> a_match_positions;
  std::set<int> b_match_positions;
  std::map<int, std::set<int>> a_b_match_positions;
  std::map<int, std::set<int>> b_a_match_positions;

  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution_.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      if (pair_class_mode == PairClassMode::kSingleton &&
          a_match_positions.count(it_a.position()) > 0 &&
          b_match_positions.count(it_b.position()) > 0) {
        // Already added both pieces.
        continue;
      }
      mutable_solution_.SetClass(it_b);
      const bool this_match = std::all_of(
          predicates.begin(),
          predicates.end(),
          [&solution](const Solution::Cropper& c) {
            return c.p(solution);
          });
      if (this_match) {
        a_match_positions.insert(it_a.position());
        b_match_positions.insert(it_b.position());
        if (pair_class_mode == PairClassMode::kMakePairs) {
          a_b_match_positions[it_a.position()].insert(it_b.position());
          b_a_match_positions[it_b.position()].insert(it_a.position());
        }
      }
    }
  }

  active_sets_[class_a] = ActiveSet(
      a_match_positions, permuter_a.permutation_count());
  active_sets_[class_b] = ActiveSet(
      b_match_positions, permuter_b.permutation_count());
  if (pair_class_mode == PairClassMode::kMakePairs) {
    for (const auto& pair : a_b_match_positions) {
      const int a_val = pair.first;
      const std::set<int>& b_set = pair.second;
      active_set_pairs_[class_a][class_b][a_val] = ActiveSet(
          b_set, permuter_b.permutation_count());
    }
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const std::set<int>& a_set = pair.second;
      active_set_pairs_[class_b][class_a][b_val] = ActiveSet(
          a_set, permuter_a.permutation_count());
    }
  }
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

void ActiveSetBuilder::Build(
    PairClassImpl pair_class_impl,
    const ClassPermuter& permuter_a,
    const ClassPermuter& permuter_b,
    const std::vector<Solution::Cropper>& predicates,
    PairClassMode pair_class_mode) {
  switch (pair_class_impl) {
  case PairClassImpl::kPairSet:
    Build<PairClassImpl::kPairSet>(permuter_a, permuter_b,
                                   predicates, pair_class_mode);
    return;
  case PairClassImpl::kBackAndForth:
    Build<PairClassImpl::kBackAndForth>(permuter_a, permuter_b,
                                        predicates, pair_class_mode);
    return;
  case PairClassImpl::kPassThroughA:
    Build<PairClassImpl::kPassThroughA>(permuter_a, permuter_b,
                                        predicates, pair_class_mode);
    return;
  default:
    LOG(FATAL) << "Bad PairClassImpl "
      << static_cast<int>(pair_class_impl);
  }
}

}  // namespace puzzle
