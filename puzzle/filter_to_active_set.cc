#include "puzzle/filter_to_active_set.h"

#include "absl/container/flat_hash_set.h"
#include "absl/flags/flag.h"
#include "puzzle/all_match.h"

ABSL_FLAG(bool, puzzle_value_skip_to_active_set, false,
          "If true, uses ValueSkipToActiveSet to restrict iterations through "
          "ClassPermuter iterations (rather than the default += ValueSkip "
          "implementation).");

namespace puzzle {

std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::SingleClassBuild val) {
  switch (val) {
    case FilterToActiveSet::SingleClassBuild::kPassThrough:
      return out << "kPassThrough";
    case FilterToActiveSet::SingleClassBuild::kPositionSet:
      return out << "kPositionSet";
  }
  return out << "Unknown SingleClassBuild(" << static_cast<int>(val) << ")";
}

std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::PairClassMode val) {
  switch (val) {
    case FilterToActiveSet::PairClassMode::kSingleton:
      return out << "kSingleton";
    case FilterToActiveSet::PairClassMode::kMakePairs:
      return out << "kMakePairs";
  }
  return out << "Unknown PairClassMode(" << static_cast<int>(val) << ")";
}

std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::PairClassImpl val) {
  switch (val) {
    case FilterToActiveSet::PairClassImpl::kPassThroughA:
      return out << "kPassThroughA";
    case FilterToActiveSet::PairClassImpl::kBackAndForth:
      return out << "kBackAndForth";
    case FilterToActiveSet::PairClassImpl::kPairSet:
      return out << "kPairSet";
  }
  return out << "Unknown PairClassImpl(" << static_cast<int>(val) << ")";
}

FilterToActiveSet::FilterToActiveSet(const EntryDescriptor* entry_descriptor,
                                     Profiler* profiler)
    : active_sets_(
          entry_descriptor == nullptr ? 0 : entry_descriptor->num_classes(),
          ActiveSet::trivial()),
      mutable_solution_(entry_descriptor),
      solution_(mutable_solution_.TestableSolution()),
      profiler_(profiler) {
  if (entry_descriptor != nullptr) {
    int num_classes = entry_descriptor->num_classes();
    active_set_pairs_.resize(num_classes);
    for (int i = 0; i < num_classes; ++i) {
      active_set_pairs_[i].resize(num_classes);
    }
  }
}

FilterToActiveSet::FilterToActiveSet(const FilterToActiveSet& other)
    : active_sets_(other.active_sets_),
      active_set_pairs_(other.active_set_pairs_),
      mutable_solution_(other.solution_.descriptor()),
      solution_(mutable_solution_.TestableSolution()) {}

void FilterToActiveSet::SetupPermuter(const ClassPermuter* class_permuter) {
  if (!absl::GetFlag(FLAGS_puzzle_value_skip_to_active_set)) return;

  const Descriptor* d = class_permuter->descriptor();
  if (value_skip_to_active_set_[d] == nullptr) {
    value_skip_to_active_set_[d] =
        absl::make_unique<ValueSkipToActiveSet>(class_permuter);
  }
}

void FilterToActiveSet::SetupBuild(
    const ClassPermuter* class_permuter,
    const std::vector<SolutionFilter>& predicates) {
  SetupPermuter(class_permuter);
  int class_int = class_permuter->class_int();
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes().size(), 1);
    CHECK_EQ(p.classes()[0], class_int);
  }
}

void FilterToActiveSet::Advance(const ValueSkipToActiveSet* vs2as,
                                ClassPermuter::iterator::ValueSkip value_skip,
                                ClassPermuter::iterator& it) const {
  if (vs2as == nullptr) {
    it += value_skip;
    return;
  }
  if (value_skip.value_index == Entry::kBadId) {
    ++it;
    return;
  }
  bool was_advanced;
  double pre_selectivity = it.Selectivity();
  it = std::move(it).WithActiveSet(vs2as->value_skip_set(it, value_skip),
                                   &was_advanced);
  VLOG(3) << pre_selectivity << " => " << it.Selectivity() << "; "
          << it.position() << "; " << value_skip.value_index;
  CHECK(was_advanced)
      << "Skipping the current value should advance the iterator";
  if (!was_advanced) {
    ++it;
  }
}

template <>
void FilterToActiveSet::Build<
    FilterToActiveSet::SingleClassBuild::kPassThrough>(
    const ClassPermuter* class_permuter,
    const std::vector<SolutionFilter>& predicates) {
  SetupBuild(class_permuter, predicates);
  int class_int = class_permuter->class_int();
  ActiveSetBuilder builder(class_permuter->permutation_count());
  ValueSkipToActiveSet* vs2as =
      value_skip_to_active_set_[class_permuter->descriptor()].get();

  ClassPermuter::iterator::ValueSkip value_skip = {.value_index =
                                                       Entry::kBadId};
  for (auto it = class_permuter->begin().WithActiveSet(active_sets_[class_int]);
       it != class_permuter->end(); Advance(vs2as, value_skip, it)) {
    mutable_solution_.SetClass(it);
    if (AllMatch(predicates, solution_, class_int, &value_skip)) {
      builder.AddBlockTo(false, it.position());
      builder.Add(true);
    }
  }
  builder.AddBlockTo(false, class_permuter->permutation_count());
  active_sets_[class_int] = builder.DoneAdding();
}

template <>
void FilterToActiveSet::Build<
    FilterToActiveSet::SingleClassBuild::kPositionSet>(
    const ClassPermuter* class_permuter,
    const std::vector<SolutionFilter>& predicates) {
  SetupBuild(class_permuter, predicates);
  int class_int = class_permuter->class_int();
  ValueSkipToActiveSet* vs2as =
      value_skip_to_active_set_[class_permuter->descriptor()].get();
  std::vector<int> a_matches;
  ClassPermuter::iterator::ValueSkip value_skip = {.value_index =
                                                       Entry::kBadId};
  for (auto it = class_permuter->begin().WithActiveSet(active_sets_[class_int]);
       it != class_permuter->end(); Advance(vs2as, value_skip, it)) {
    mutable_solution_.SetClass(it);
    if (AllMatch(predicates, solution_, class_int, &value_skip)) {
      a_matches.push_back(it.position());
    }
  }
  active_sets_[class_int] = ActiveSetBuilder::FromPositions(
      a_matches, class_permuter->permutation_count());
}

void FilterToActiveSet::SetupPairBuild(
    const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
    const std::vector<SolutionFilter>& predicates) {
  SetupPermuter(permuter_a);
  SetupPermuter(permuter_b);
  int class_a = permuter_a->class_int();
  int class_b = permuter_b->class_int();
  for (const auto& p : predicates) {
    CHECK_EQ(p.classes().size(), 2);
    CHECK(p.classes()[0] == class_a || p.classes()[0] == class_b);
    CHECK(p.classes()[1] == class_a || p.classes()[1] == class_b);
    CHECK_NE(p.classes()[0], p.classes()[1]);
  }
}

template <typename Fn1, typename Fn2, typename Fn3>
void FilterToActiveSet::DualIterate(const ClassPermuter* outer,
                                    const ClassPermuter* inner,
                                    Fn1 on_outer_before, Fn2 on_inner,
                                    Fn3 on_outer_after) {
  // TODO(@monkeynova): Can we track a potential "value_skip_b" which
  //                    indicates that all of the values of a failed because
  //                    of a skipable b value?
  // TODO(@monkeynova): We might do well to have a predicates_a and
  //                    predicates_b that allow for sorting by the entry_id
  //                    for the respective classes.
  const int class_outer = outer->class_int();
  const int class_inner = inner->class_int();
  ActiveSetPair& outer_inner_pair = active_set_pairs_[class_outer][class_inner];
  ValueSkipToActiveSet* vs2as_inner =
      value_skip_to_active_set_[inner->descriptor()].get();

  for (auto it_outer = outer->begin().WithActiveSet(active_sets_[class_outer]);
       it_outer != outer->end(); ++it_outer) {
    mutable_solution_.SetClass(it_outer);
    on_outer_before();
    ClassPermuter::iterator::ValueSkip value_skip_inner = {.value_index =
                                                               Entry::kBadId};
    for (auto it_inner =
             inner->begin()
                 .WithActiveSet(active_sets_[class_inner])
                 .WithActiveSet(outer_inner_pair.Find(it_outer.position()));
         it_inner != inner->end();
         Advance(vs2as_inner, value_skip_inner, it_inner)) {
      if (profiler_ != nullptr) {
        profiler_->NotePrepare(
            inner->permutation_count() * it_outer.position() +
                it_inner.position(),
            inner->permutation_count() * outer->permutation_count());
      }
      mutable_solution_.SetClass(it_inner);
      if (on_inner(it_outer, it_inner, &value_skip_inner)) break;
    }
    on_outer_after(it_outer);
  }
}

template <>
void FilterToActiveSet::Build<FilterToActiveSet::PairClassImpl::kBackAndForth>(
    const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
    const std::vector<SolutionFilter>& predicates,
    FilterToActiveSet::PairClassMode pair_class_mode) {
  SetupPairBuild(permuter_a, permuter_b, predicates);
  int class_a = permuter_a->class_int();
  int class_b = permuter_b->class_int();
  ActiveSetPair& a_b_pair = active_set_pairs_[class_a][class_b];
  ActiveSetPair& b_a_pair = active_set_pairs_[class_b][class_a];
  // Since we expect 'a' to be the smaller of the iterations, we use it as the
  // inner loop first, hoping to prune 'b' for its iteration.
  {
    ActiveSetBuilder builder_b(permuter_b->permutation_count());
    bool any_of_a;
    ActiveSetBuilder b_a_builder(permuter_a->permutation_count());

    DualIterate(
        permuter_b, permuter_a,
        // Outer, before inner.
        [&]() {
          b_a_builder = ActiveSetBuilder(permuter_a->permutation_count());
          any_of_a = false;
        },
        // Inner.
        [&](const ClassPermuter::iterator& it_b,
            const ClassPermuter::iterator& it_a,
            ClassPermuter::iterator::ValueSkip* value_skip) {
          if (AllMatch(predicates, solution_, class_a, value_skip)) {
            any_of_a = true;
            if (pair_class_mode == PairClassMode::kSingleton) return true;
            if (pair_class_mode == PairClassMode::kMakePairs) {
              b_a_builder.AddBlockTo(false, it_a.position());
              b_a_builder.Add(true);
            }
          }
          return false;
        },
        // Outer, after inner.
        [&](const ClassPermuter::iterator& it_b) {
          if (any_of_a) {
            builder_b.AddBlockTo(false, it_b.position());
            builder_b.Add(true);
          }
          if (pair_class_mode == PairClassMode::kMakePairs) {
            b_a_builder.AddBlockTo(false, permuter_a->permutation_count());
            b_a_pair.Assign(it_b.position(), b_a_builder.DoneAdding());
          }
        });

    builder_b.AddBlockTo(false, permuter_b->permutation_count());
    active_sets_[class_b] = builder_b.DoneAdding();
  }
  {
    ActiveSetBuilder builder_a(permuter_a->permutation_count());
    bool any_of_b;
    ActiveSetBuilder a_b_builder(permuter_b->permutation_count());

    DualIterate(
        permuter_a, permuter_b,
        // Outer, before inner.
        [&]() {
          a_b_builder = ActiveSetBuilder(permuter_b->permutation_count());
          any_of_b = false;
        },
        // Inner.
        [&](const ClassPermuter::iterator& it_a,
            const ClassPermuter::iterator& it_b,
            ClassPermuter::iterator::ValueSkip* value_skip) {
          if (AllMatch(predicates, solution_, class_b, value_skip)) {
            any_of_b = true;
            if (pair_class_mode == PairClassMode::kSingleton) return true;
            if (pair_class_mode == PairClassMode::kMakePairs) {
              a_b_builder.AddBlockTo(false, it_b.position());
              a_b_builder.Add(true);
            }
          }
          return false;
        },
        // Outer, after inner.
        [&](const ClassPermuter::iterator& it_a) {
          if (any_of_b) {
            builder_a.AddBlockTo(false, it_a.position());
            builder_a.Add(true);
          }
          if (pair_class_mode == PairClassMode::kMakePairs) {
            a_b_builder.AddBlockTo(false, permuter_b->permutation_count());
            a_b_pair.Assign(it_a.position(), a_b_builder.DoneAdding());
          }
        });

    builder_a.AddBlockTo(false, permuter_a->permutation_count());
    active_sets_[class_a] = builder_a.DoneAdding();
  }
}

template <>
void FilterToActiveSet::Build<FilterToActiveSet::PairClassImpl::kPassThroughA>(
    const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
    const std::vector<SolutionFilter>& predicates,
    FilterToActiveSet::PairClassMode pair_class_mode) {
  SetupPairBuild(permuter_a, permuter_b, predicates);
  int class_a = permuter_a->class_int();
  int class_b = permuter_b->class_int();
  ActiveSetPair& a_b_pair = active_set_pairs_[class_a][class_b];
  ActiveSetPair& b_a_pair = active_set_pairs_[class_b][class_a];
  ActiveSetBuilder builder_a(permuter_a->permutation_count());
  absl::flat_hash_set<int> b_match_positions;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> b_a_match_positions;

  ActiveSetBuilder a_b_builder(permuter_b->permutation_count());
  bool any_of_b;
  DualIterate(
      permuter_a, permuter_b,
      // Outer before inner.
      [&]() {
        any_of_b = false;
        a_b_builder = ActiveSetBuilder(permuter_b->permutation_count());
      },
      // Inner.
      [&](const ClassPermuter::iterator& it_a,
          const ClassPermuter::iterator& it_b,
          ClassPermuter::iterator::ValueSkip* value_skip) {
        if (pair_class_mode == PairClassMode::kSingleton && any_of_b &&
            b_match_positions.find(it_b.position()) !=
                b_match_positions.end()) {
          // Already added both pieces.
          return true;
        }
        if (AllMatch(predicates, solution_, class_b, value_skip)) {
          any_of_b = true;
          b_match_positions.insert(it_b.position());
          if (pair_class_mode == PairClassMode::kMakePairs) {
            a_b_builder.AddBlockTo(false, it_b.position());
            a_b_builder.Add(true);
            b_a_match_positions[it_b.position()].insert(it_a.position());
          }
        }
        return false;
      },
      // Outer after inner.
      [&](const ClassPermuter::iterator& it_a) {
        if (pair_class_mode == PairClassMode::kMakePairs) {
          a_b_builder.AddBlockTo(false, permuter_b->permutation_count());
          a_b_pair.Assign(it_a.position(), a_b_builder.DoneAdding());
        }
        if (any_of_b) {
          builder_a.AddBlockTo(false, it_a.position());
          builder_a.Add(true);
        }
      });

  builder_a.AddBlockTo(false, permuter_a->permutation_count());
  active_sets_[class_a] = builder_a.DoneAdding();
  active_sets_[class_b] = ActiveSetBuilder::FromPositions(
      b_match_positions, permuter_b->permutation_count());
  if (pair_class_mode == PairClassMode::kMakePairs) {
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const absl::flat_hash_set<int>& a_set = pair.second;
      b_a_pair.Assign(b_val, ActiveSetBuilder::FromPositions(
                                 a_set, permuter_a->permutation_count()));
    }
  }
}

template <>
void FilterToActiveSet::Build<FilterToActiveSet::PairClassImpl::kPairSet>(
    const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
    const std::vector<SolutionFilter>& predicates,
    FilterToActiveSet::PairClassMode pair_class_mode) {
  SetupPairBuild(permuter_a, permuter_b, predicates);
  int class_a = permuter_a->class_int();
  int class_b = permuter_b->class_int();
  ActiveSetPair& a_b_pair = active_set_pairs_[class_a][class_b];
  ActiveSetPair& b_a_pair = active_set_pairs_[class_b][class_a];
  Solution solution = mutable_solution_.TestableSolution();
  absl::flat_hash_set<int> a_match_positions;
  absl::flat_hash_set<int> b_match_positions;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> a_b_match_positions;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> b_a_match_positions;

  DualIterate(
      permuter_a, permuter_b,
      // Outer before inner.
      [&]() {},
      // Inner.
      [&](const ClassPermuter::iterator& it_a,
          const ClassPermuter::iterator& it_b,
          ClassPermuter::iterator::ValueSkip* value_skip) {
        if (pair_class_mode == PairClassMode::kSingleton &&
            a_match_positions.count(it_a.position()) > 0 &&
            b_match_positions.count(it_b.position()) > 0) {
          // Already added both pieces.
          return true;
        }
        if (AllMatch(predicates, solution_, class_b, value_skip)) {
          a_match_positions.insert(it_a.position());
          b_match_positions.insert(it_b.position());
          if (pair_class_mode == PairClassMode::kMakePairs) {
            a_b_match_positions[it_a.position()].insert(it_b.position());
            b_a_match_positions[it_b.position()].insert(it_a.position());
          }
        }
        return false;
      },
      // Outer after inner.
      [&](const ClassPermuter::iterator& it_a) {});

  active_sets_[class_a] = ActiveSetBuilder::FromPositions(
      a_match_positions, permuter_a->permutation_count());
  active_sets_[class_b] = ActiveSetBuilder::FromPositions(
      b_match_positions, permuter_b->permutation_count());
  if (pair_class_mode == PairClassMode::kMakePairs) {
    for (const auto& pair : a_b_match_positions) {
      const int a_val = pair.first;
      const absl::flat_hash_set<int>& b_set = pair.second;
      a_b_pair.Assign(a_val, ActiveSetBuilder::FromPositions(
                                 b_set, permuter_b->permutation_count()));
    }
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const absl::flat_hash_set<int>& a_set = pair.second;
      b_a_pair.Assign(b_val, ActiveSetBuilder::FromPositions(
                                 a_set, permuter_a->permutation_count()));
    }
  }
}

void FilterToActiveSet::Build(SingleClassBuild single_class_build,
                              const ClassPermuter* class_permuter,
                              const std::vector<SolutionFilter>& predicates) {
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

void FilterToActiveSet::Build(PairClassImpl pair_class_impl,
                              const ClassPermuter* permuter_a,
                              const ClassPermuter* permuter_b,
                              const std::vector<SolutionFilter>& predicates,
                              PairClassMode pair_class_mode) {
  switch (pair_class_impl) {
    case PairClassImpl::kPairSet:
      Build<PairClassImpl::kPairSet>(permuter_a, permuter_b, predicates,
                                     pair_class_mode);
      return;
    case PairClassImpl::kBackAndForth:
      Build<PairClassImpl::kBackAndForth>(permuter_a, permuter_b, predicates,
                                          pair_class_mode);
      return;
    case PairClassImpl::kPassThroughA:
      Build<PairClassImpl::kPassThroughA>(permuter_a, permuter_b, predicates,
                                          pair_class_mode);
      return;
    default:
      LOG(FATAL) << "Bad PairClassImpl " << static_cast<int>(pair_class_impl);
  }
}

}  // namespace puzzle
