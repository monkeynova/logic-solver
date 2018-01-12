#include "puzzle/active_set_builder.h"

#include "benchmark/benchmark.h"

namespace puzzle {

struct SetupState {
  static const int kClassIntA = 0;
  static const int kClassIntB = 1;

  enum BuildMode {
    kEarlyExit = 1,
    kFullIteration = 2,
    kMakePairs = 3,
  };

  SetupState(int permutation_count, BuildMode build_mode_in)
    : build_mode(build_mode_in),
      descriptor(MakeDescriptor(permutation_count, &owned_descriptors)),
      mutable_solution(&descriptor),
      solution(mutable_solution.TestableSolution()),
      predicates({MakePairCropper()}),
      permuter_a(descriptor.AllClassValues(kClassIntA), kClassIntA),
      permuter_b(descriptor.AllClassValues(kClassIntB), kClassIntB),
      active_sets(descriptor.num_classes()),
      active_set_pairs(descriptor.num_classes()) {
    ActiveSetBuilder single_class_builder(&descriptor);
    single_class_builder.Build(permuter_a, {MakeCropperA()});
    permuter_a.set_active_set(single_class_builder.active_set(kClassIntA));
    single_class_builder.Build(permuter_b, {MakeCropperB()});
    permuter_b.set_active_set(single_class_builder.active_set(kClassIntB));
  }

  static EntryDescriptor MakeDescriptor(
      int permutation_count,
      std::vector<std::unique_ptr<Descriptor>>* descriptors) {
    EntryDescriptor ret;
    descriptors->emplace_back(new IntRangeDescriptor(0, permutation_count));
    ret.SetIds(descriptors->back().get());
    ret.SetClass(kClassIntA, "Class A", descriptors->back().get());
    ret.SetClass(kClassIntB, "Class B", descriptors->back().get());
    return ret;
  }

  static Solution::Cropper MakePairCropper() {
    Solution::Cropper ret(
        "no entry the same",
        [](const Solution& s) {
          return all_of(s.entries().begin(),
                        s.entries().end(),
                              [](const Entry& e) {
                          return e.Class(kClassIntA) != e.Class(kClassIntB);
                        });
        },
        {kClassIntA, kClassIntB});
    return ret;
  }

  static Solution::Cropper MakeCropperA() {
    Solution::Cropper ret(
        "no entry the same",
        [](const Solution& s) {
          return s.Id(0).Class(kClassIntA) == 0 && s.Id(1).Class(kClassIntA) == 1;
        },
        {kClassIntA});
    return ret;
  }

  static Solution::Cropper MakeCropperB() {
    Solution::Cropper ret(
        "no entry the same",
        [](const Solution& s) {
          return s.Id(0).Class(kClassIntB) == 1 && s.Id(1).Class(kClassIntB) == 0;
        },
        {kClassIntB});
    return ret;
  }

  void BuildPairSet();
  void BuildPassThroughA();
  void BuildBackAndForth();

  BuildMode build_mode;
  std::vector<std::unique_ptr<Descriptor>> owned_descriptors;
  EntryDescriptor descriptor;
  MutableSolution mutable_solution;
  Solution solution;
  std::vector<Solution::Cropper> predicates;
  ClassPermuter permuter_a;
  ClassPermuter permuter_b;
  std::vector<ActiveSet> active_sets;
  std::vector<std::map<int, ActiveSet>> active_set_pairs;
};

template <SetupState::BuildMode build_mode>
static void BM_PairSet(benchmark::State& state) {
  SetupState setup(state.range(0), build_mode);

  for (auto _ : state) {
    setup.BuildPairSet();
  }
}

void SetupState::BuildPairSet() {
  std::set<int> a_match_positions;
  std::set<int> b_match_positions;
  std::map<int, std::set<int>> a_b_match_positions;
  std::map<int, std::set<int>> b_a_match_positions;

  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      if (build_mode == SetupState::kEarlyExit &&
          a_match_positions.count(it_a.position()) > 0 &&
          b_match_positions.count(it_b.position()) > 0) {
        // Already added both pieces.
        continue;
      }
      mutable_solution.SetClass(it_b);
      const bool this_match = std::all_of(predicates.begin(),
                                          predicates.end(),
                                          [this](const Solution::Cropper& c) {
                                            return c.p(solution);
                                          });
      if (this_match) {
        a_match_positions.insert(it_a.position());
        b_match_positions.insert(it_b.position());
        if (build_mode == SetupState::kMakePairs) {
          a_b_match_positions[it_a.position()].insert(it_b.position());
          b_a_match_positions[it_b.position()].insert(it_a.position());
        }
      }
    }
  }

  active_sets[permuter_a.class_int()] = ActiveSet(
      a_match_positions, permuter_a.permutation_count());
  active_sets[permuter_b.class_int()] = ActiveSet(
      b_match_positions, permuter_b.permutation_count());
  if (build_mode == SetupState::kMakePairs) {
    for (const auto& pair : a_b_match_positions) {
      const int a_val = pair.first;
      const std::set<int>& b_set = pair.second;
      active_set_pairs[SetupState::kClassIntA][a_val] = ActiveSet(
          b_set, permuter_b.permutation_count());
    }
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const std::set<int>& a_set = pair.second;
      active_set_pairs[SetupState::kClassIntB][b_val] = ActiveSet(
          a_set, permuter_a.permutation_count());
    }
  }
}

template <SetupState::BuildMode build_mode>
static void BM_PairPassThroughA(benchmark::State& state) {
  SetupState setup(state.range(0), build_mode);

  for (auto _ : state) {
    setup.BuildPassThroughA();
  }
}

void SetupState::BuildPassThroughA() {
  ActiveSet active_set_a = ActiveSet();
  ActiveSet source_a = permuter_a.active_set();
  std::set<int> b_match_positions;
  std::map<int, std::set<int>> b_a_match_positions;

  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    bool any_of_a = false;
    ActiveSet a_b_set;
    ActiveSet source_b = permuter_b.active_set();
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      if (build_mode == SetupState::kEarlyExit && any_of_a &&
          b_match_positions.find(it_b.position()) != b_match_positions.end()) {
        // Already added both pieces.
        continue;
      }
      mutable_solution.SetClass(it_b);
      const bool this_match = std::all_of(predicates.begin(),
                                          predicates.end(),
                                          [this](const Solution::Cropper& c) {
                                            return c.p(solution);
                                          });
      if (this_match) {
        any_of_a = true;
        b_match_positions.insert(it_b.position());
      }
      if (build_mode == SetupState::kMakePairs) {
        a_b_set.AddFalseBlock(source_b.ConsumeFalseBlock());
        CHECK(source_b.ConsumeNext());
        a_b_set.Add(this_match);
        if (this_match) {
          b_a_match_positions[it_b.position()].insert(it_a.position());
        }
      }
    }
    if (build_mode == SetupState::kMakePairs && any_of_a) {
      a_b_set.DoneAdding();
      active_set_pairs[SetupState::kClassIntA][it_a.position()] =
        std::move(a_b_set);
    }
    active_set_a.Add(any_of_a);
  }

  active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
  CHECK(source_a.ConsumeNext());
  active_set_a.DoneAdding();
  active_sets[permuter_a.class_int()] = std::move(active_set_a);
  active_sets[permuter_b.class_int()] = ActiveSet(
      b_match_positions, permuter_b.permutation_count());
  if (build_mode == SetupState::kMakePairs) {
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const std::set<int>& a_set = pair.second;
      active_set_pairs[SetupState::kClassIntB][b_val] = ActiveSet(
          a_set, permuter_a.permutation_count());
    }
  }
}

template <SetupState::BuildMode build_mode>
static void BM_PairBackAndForth(benchmark::State& state) {
  SetupState setup(state.range(0), build_mode);

  for (auto _ : state) {
    setup.BuildBackAndForth();
  }
}

void SetupState::BuildBackAndForth() {
  {
    ActiveSet active_set_a = ActiveSet();
    ActiveSet source_a = permuter_a.active_set();

    for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
      mutable_solution.SetClass(it_a);
      active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
      CHECK(source_a.ConsumeNext());
      bool any_of_a = false;
      ActiveSet a_b_set;
      ActiveSet source_b = permuter_b.active_set();
      for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
        mutable_solution.SetClass(it_b);
        const bool this_match = std::all_of(predicates.begin(),
                                            predicates.end(),
                                            [this](const Solution::Cropper& c) {
                                              return c.p(solution);
                                            });
        if (this_match) {
          any_of_a = true;
          if (build_mode == SetupState::kEarlyExit) break;
        }
        if (build_mode == SetupState::kMakePairs) {
          a_b_set.AddFalseBlock(source_b.ConsumeFalseBlock());
          CHECK(source_b.ConsumeNext());
          a_b_set.Add(this_match);
        }
      }
      active_set_a.Add(any_of_a);
      if (build_mode == SetupState::kMakePairs && any_of_a) {
        a_b_set.DoneAdding();
        active_set_pairs[SetupState::kClassIntA][it_a.position()] =
          std::move(a_b_set);
      }
    }

    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    active_set_a.DoneAdding();
    active_sets[permuter_a.class_int()] = std::move(active_set_a);
  }
  {
    ActiveSet active_set_b = ActiveSet();
    ActiveSet source_b = permuter_b.active_set();

    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
      CHECK(source_b.ConsumeNext());
      ActiveSet b_a_set;
      ActiveSet source_a = permuter_a.active_set();
      bool any_of_b = false;
      for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
        mutable_solution.SetClass(it_a);
        const bool this_match = std::all_of(predicates.begin(),
                                            predicates.end(),
                                            [this](const Solution::Cropper& c) {
                                              return c.p(solution);
                                            });
        if (this_match) {
          any_of_b = true;
          if (build_mode == SetupState::kEarlyExit) break;
        }
        if (build_mode == SetupState::kMakePairs) {
          b_a_set.AddFalseBlock(source_a.ConsumeFalseBlock());
          CHECK(source_a.ConsumeNext());
          b_a_set.Add(this_match);
        }
      }
      active_set_b.Add(any_of_b);
      if (build_mode == SetupState::kMakePairs && any_of_b) {
        b_a_set.DoneAdding();
        active_set_pairs[SetupState::kClassIntB][it_b.position()] =
          std::move(b_a_set);
      }
    }

    active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
    CHECK(source_b.ConsumeNext());
    active_set_b.DoneAdding();
    active_sets[permuter_b.class_int()] = std::move(active_set_b);
  }
}

BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kEarlyExit)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kEarlyExit)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kEarlyExit)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kFullIteration)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kFullIteration)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kFullIteration)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kMakePairs)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kMakePairs)->Arg(3);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kMakePairs)->Arg(3);

BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kEarlyExit)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kEarlyExit)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kEarlyExit)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kFullIteration)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kFullIteration)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kFullIteration)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kMakePairs)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kMakePairs)->Arg(5);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kMakePairs)->Arg(5);

BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kEarlyExit)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kEarlyExit)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kEarlyExit)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kFullIteration)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kFullIteration)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kFullIteration)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairSet, SetupState::kMakePairs)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairPassThroughA, SetupState::kMakePairs)->Arg(7);
BENCHMARK_TEMPLATE(BM_PairBackAndForth, SetupState::kMakePairs)->Arg(7);

}  // namespace puzzle
