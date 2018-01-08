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
  
  SetupState(int permutation_count)
    : descriptor(MakeDescriptor(permutation_count, &owned_descriptors)),
      mutable_solution(&descriptor),
      solution(mutable_solution.TestableSolution()),
      predicates({MakeCropper()}),
      permuter_a(descriptor.AllClassValues(kClassIntA), kClassIntA),
      permuter_b(descriptor.AllClassValues(kClassIntB), kClassIntB),
      active_sets(descriptor.num_classes()),
      active_set_pairs(descriptor.num_classes()) {}

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

  static Solution::Cropper MakeCropper() {
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
  SetupState setup(state.range(0));

  for (auto _ : state) {
    std::set<int> a_match_positions;
    std::set<int> b_match_positions;
    std::map<int, std::set<int>> a_b_match_positions;
    std::map<int, std::set<int>> b_a_match_positions;

    for (auto it_a = setup.permuter_a.begin();
         it_a != setup.permuter_a.end();
         ++it_a) {
      setup.mutable_solution.SetClass(it_a);
      for (auto it_b = setup.permuter_b.begin();
           it_b != setup.permuter_b.end();
           ++it_b) {
	if (build_mode == SetupState::kEarlyExit &&
	    a_match_positions.count(it_a.position()) > 0 &&
	    b_match_positions.count(it_b.position()) > 0) {
          // Already added both pieces.
	  continue;
	}
        setup.mutable_solution.SetClass(it_b);
        const bool this_match = std::all_of(setup.predicates.begin(),
                                            setup.predicates.end(),
                                            [&setup](const Solution::Cropper& c) {
                                              return c.p(setup.solution);
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

    setup.active_sets[setup.permuter_a.class_int()] = ActiveSet(
        a_match_positions, setup.permuter_a.permutation_count());
    for (const auto& pair : a_b_match_positions) {
      const int a_val = pair.first;
      const std::set<int>& b_set = pair.second;
      setup.active_set_pairs[SetupState::kClassIntA][a_val] = ActiveSet(
  	  b_set, setup.permuter_b.permutation_count());
    }
    setup.active_sets[setup.permuter_b.class_int()] = ActiveSet(
        b_match_positions, setup.permuter_b.permutation_count());
    for (const auto& pair : b_a_match_positions) {
      const int b_val = pair.first;
      const std::set<int>& a_set = pair.second;
      setup.active_set_pairs[SetupState::kClassIntB][b_val] = ActiveSet(
  	  a_set, setup.permuter_a.permutation_count());
    }
  }
}

template <SetupState::BuildMode build_mode>
static void BM_PairPassThroughA(benchmark::State& state) {
  SetupState setup(state.range(0));

  for (auto _ : state) {
    ActiveSet active_set_a = ActiveSet();
    ActiveSet source_a = setup.permuter_a.active_set();
    std::set<int> b_match_positions;

    for (auto it_a = setup.permuter_a.begin();
         it_a != setup.permuter_a.end();
         ++it_a) {
      setup.mutable_solution.SetClass(it_a);
      active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
      CHECK(source_a.ConsumeNext());
      bool any_of_a = false;
      for (auto it_b = setup.permuter_b.begin();
           it_b != setup.permuter_b.end();
           ++it_b) {
        if (build_mode == SetupState::kEarlyExit && any_of_a &&
            b_match_positions.find(it_b.position()) != b_match_positions.end()) {
          // Already added both pieces.
          continue;
        }
        setup.mutable_solution.SetClass(it_b);
        const bool this_match = std::all_of(setup.predicates.begin(),
                                            setup.predicates.end(),
                                            [&setup](const Solution::Cropper& c) {
                                              return c.p(setup.solution);
                                            });
        if (this_match) {
          any_of_a = true;
          b_match_positions.insert(it_b.position());
        }
      }
      active_set_a.Add(any_of_a);
    }

    active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
    CHECK(source_a.ConsumeNext());
    active_set_a.DoneAdding();
    setup.active_sets[setup.permuter_a.class_int()] = std::move(active_set_a);
    setup.active_sets[setup.permuter_b.class_int()] = ActiveSet(
        b_match_positions, setup.permuter_b.permutation_count());
  }
}

template <SetupState::BuildMode build_mode>
static void BM_PairBackAndForth(benchmark::State& state) {
  SetupState setup(state.range(0));

  for (auto _ : state) {
    ActiveSet active_set_a = ActiveSet();
    ActiveSet active_set_b = ActiveSet();

    {
      ActiveSet source_a = setup.permuter_a.active_set();

      for (auto it_a = setup.permuter_a.begin();
           it_a != setup.permuter_a.end();
           ++it_a) {
        setup.mutable_solution.SetClass(it_a);
        active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
        CHECK(source_a.ConsumeNext());
        bool any_of_a = false;
        for (auto it_b = setup.permuter_b.begin();
             it_b != setup.permuter_b.end();
             ++it_b) {
          setup.mutable_solution.SetClass(it_b);
          const bool this_match = std::all_of(setup.predicates.begin(),
                                              setup.predicates.end(),
                                              [&setup](const Solution::Cropper& c) {
                                                return c.p(setup.solution);
                                              });
          if (this_match) {
            any_of_a = true;
            if (build_mode == SetupState::kEarlyExit) break;
          }
        }
        active_set_a.Add(any_of_a);
      }

      active_set_a.AddFalseBlock(source_a.ConsumeFalseBlock());
      CHECK(source_a.ConsumeNext());
      active_set_a.DoneAdding();
    }
    {
      ActiveSet source_b = setup.permuter_b.active_set();

      for (auto it_b = setup.permuter_b.begin();
           it_b != setup.permuter_b.end();
           ++it_b) {
        setup.mutable_solution.SetClass(it_b);
        active_set_a.AddFalseBlock(source_b.ConsumeFalseBlock());
        CHECK(source_b.ConsumeNext());
        bool any_of_b = false;
        for (auto it_a = setup.permuter_a.begin();
             it_a != setup.permuter_a.end();
             ++it_a) {
          setup.mutable_solution.SetClass(it_a);
          const bool this_match = std::all_of(setup.predicates.begin(),
                                              setup.predicates.end(),
                                              [&setup](const Solution::Cropper& c) {
                                                return c.p(setup.solution);
                                              });
          if (this_match) {
            any_of_b = true;
            if (build_mode == SetupState::kEarlyExit) break;
          }
        }
        active_set_b.Add(any_of_b);
      }

      active_set_b.AddFalseBlock(source_b.ConsumeFalseBlock());
      CHECK(source_b.ConsumeNext());
      active_set_b.DoneAdding();
    }

    setup.active_sets[setup.permuter_a.class_int()] = std::move(active_set_a);
    setup.active_sets[setup.permuter_b.class_int()] = std::move(active_set_b);
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

}  // namespace puzzle
