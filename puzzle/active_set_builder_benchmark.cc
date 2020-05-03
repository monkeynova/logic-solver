#include "benchmark/benchmark.h"
#include "puzzle/active_set_builder.h"

namespace puzzle {

struct SetupState {
  static const int kClassIntA = 0;
  static const int kClassIntB = 1;

  SetupState(int permutation_count)
      : descriptor(MakeDescriptor(permutation_count, &owned_descriptors)),
        predicates({MakePairCropper()}),
        permuter_a(descriptor.AllClassValues(kClassIntA), kClassIntA),
        permuter_b(descriptor.AllClassValues(kClassIntB), kClassIntB) {
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
          return all_of(s.entries().begin(), s.entries().end(),
                        [](const Entry& e) {
                          return e.Class(kClassIntA) != e.Class(kClassIntB);
                        });
        },
        {kClassIntA, kClassIntB});
    return ret;
  }

  static Solution::Cropper MakeCropperA() {
    Solution::Cropper ret("no entry the same",
                          [](const Solution& s) {
                            return s.Id(0).Class(kClassIntA) == 0 &&
                                   s.Id(1).Class(kClassIntA) == 1;
                          },
                          {kClassIntA});
    return ret;
  }

  static Solution::Cropper MakeCropperB() {
    Solution::Cropper ret("no entry the same",
                          [](const Solution& s) {
                            return s.Id(0).Class(kClassIntB) == 1 &&
                                   s.Id(1).Class(kClassIntB) == 0;
                          },
                          {kClassIntB});
    return ret;
  }

  void BuildPairSet();
  void BuildPassThroughA();
  void BuildBackAndForth();

  std::vector<std::unique_ptr<Descriptor>> owned_descriptors;
  EntryDescriptor descriptor;
  std::vector<Solution::Cropper> predicates;
  ClassPermuter permuter_a;
  ClassPermuter permuter_b;
};

template <ActiveSetBuilder::PairClassImpl pair_class_impl,
          ActiveSetBuilder::PairClassMode pair_class_mode>
static void BM_Pair(benchmark::State& state) {
  SetupState setup(state.range(0));

  for (auto _ : state) {
    ActiveSetBuilder builder(&setup.descriptor);
    builder.Build<pair_class_impl>(setup.permuter_a, setup.permuter_b,
                                   setup.predicates, pair_class_mode);
  }
}

BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kPairSet,
                   ActiveSetBuilder::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kPassThroughA,
                   ActiveSetBuilder::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kBackAndForth,
                   ActiveSetBuilder::PairClassMode::kSingleton)
    ->Arg(5)
    ->Arg(7);

BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kPairSet,
                   ActiveSetBuilder::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kPassThroughA,
                   ActiveSetBuilder::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);
BENCHMARK_TEMPLATE(BM_Pair, ActiveSetBuilder::PairClassImpl::kBackAndForth,
                   ActiveSetBuilder::PairClassMode::kMakePairs)
    ->Arg(5)
    ->Arg(7);

}  // namespace puzzle
