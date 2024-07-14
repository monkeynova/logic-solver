#include "benchmark/benchmark.h"
#include "puzzle/base/solution_filter.h"
#include "puzzle/class_permuter/factory.h"
#include "puzzle/solution_permuter/filter_to_active_set.h"

namespace puzzle {

struct SetupState {
  static const int kClassIntA = 0;
  static const int kClassIntB = 1;

  SetupState(int permutation_count)
      : descriptor(MakeDescriptor(permutation_count)),
        predicates({MakePairFilter()}),
        permuter_a(MakeClassPermuter(descriptor.AllClassValues(kClassIntA),
                                     kClassIntA)),
        permuter_b(MakeClassPermuter(descriptor.AllClassValues(kClassIntB),
                                     kClassIntB)),
        single_class_builder(&descriptor) {
    CHECK(single_class_builder.Build(permuter_a.get(), {MakeFilterA()}).ok());
    CHECK(single_class_builder.Build(permuter_b.get(), {MakeFilterB()}).ok());
  }

  static EntryDescriptor MakeDescriptor(int permutation_count) {
    std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
    class_descriptors.push_back(
        absl::make_unique<IntRangeDescriptor>(permutation_count));
    class_descriptors.push_back(
        absl::make_unique<IntRangeDescriptor>(permutation_count));
    return EntryDescriptor(
        absl::make_unique<IntRangeDescriptor>(permutation_count),
        absl::make_unique<StringDescriptor>(
            std::vector<std::string>{"Class A", "Class B"}),
        std::move(class_descriptors));
  }

  static SolutionFilter MakePairFilter() {
    SolutionFilter ret("no entry the same",
                       [](const SolutionView& s) {
                         return all_of(s.entries().begin(), s.entries().end(),
                                       [](const Entry& e) {
                                         return e.Class(kClassIntA) !=
                                                e.Class(kClassIntB);
                                       });
                       },
                       {kClassIntA, kClassIntB});
    return ret;
  }

  static SolutionFilter MakeFilterA() {
    SolutionFilter ret("no entry the same",
                       [](const SolutionView& s) {
                         return s.Id(0).Class(kClassIntA) == 0 &&
                                s.Id(1).Class(kClassIntA) == 1;
                       },
                       {kClassIntA});
    return ret;
  }

  static SolutionFilter MakeFilterB() {
    SolutionFilter ret("no entry the same",
                       [](const SolutionView& s) {
                         return s.Id(0).Class(kClassIntB) == 1 &&
                                s.Id(1).Class(kClassIntB) == 0;
                       },
                       {kClassIntB});
    return ret;
  }

  void BuildPairSet();
  void BuildPassThroughA();
  void BuildBackAndForth();

  EntryDescriptor descriptor;
  std::vector<SolutionFilter> predicates;
  std::unique_ptr<ClassPermuter> permuter_a;
  std::unique_ptr<ClassPermuter> permuter_b;
  FilterToActiveSet single_class_builder;
};

template <FilterToActiveSet::PairClassImpl pair_class_impl,
          FilterToActiveSet::PairClassMode pair_class_mode>
static void BM_Pair(benchmark::State& state) {
  SetupState setup(state.range(0));

  for (auto _ : state) {
    FilterToActiveSet builder = setup.single_class_builder;
    CHECK(builder
              .Build<pair_class_impl>(setup.permuter_a.get(),
                                      setup.permuter_b.get(), setup.predicates,
                                      pair_class_mode)
              .ok());
  }
}

BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kPairSet,
                   FilterToActiveSet::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kPassThroughA,
                   FilterToActiveSet::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kBackAndForth,
                   FilterToActiveSet::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);

BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kPairSet,
                   FilterToActiveSet::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kPassThroughA,
                   FilterToActiveSet::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, FilterToActiveSet::PairClassImpl::kBackAndForth,
                   FilterToActiveSet::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);

}  // namespace puzzle
