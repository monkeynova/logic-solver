#include "puzzle/filter_to_active_set.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set.h"
#include "puzzle/class_permuter.h"
#include "puzzle/class_permuter_factory.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution_filter.h"

using ::testing::ElementsAreArray;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::Le;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

namespace puzzle {

class SinglePermuterTest
    : public ::testing::TestWithParam<FilterToActiveSet::SingleClassBuild> {
 public:
  FilterToActiveSet::SingleClassBuild single_class_build() {
    return GetParam();
  }
};

INSTANTIATE_TEST_SUITE_P(
    Instantiation, SinglePermuterTest,
    testing::Values(FilterToActiveSet::SingleClassBuild::kPassThrough,
                    FilterToActiveSet::SingleClassBuild::kPositionSet));

TEST_P(SinglePermuterTest, Simple) {
  static constexpr int kClassInt = 0;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(absl::make_unique<IntRangeDescriptor>(3),
                                   absl::make_unique<StringDescriptor>(
                                       std::vector<std::string>{"test class"}),
                                   std::move(class_descriptors));
  const Descriptor* class_descriptor =
      entry_descriptor.AllClassValues(kClassInt);

  std::unique_ptr<ClassPermuter> p(
      MakeClassPermuter(class_descriptor, kClassInt));
  ASSERT_THAT(p->permutation_count(), 6);

  FilterToActiveSet first_is_0_builder(&entry_descriptor);
  ASSERT_TRUE(first_is_0_builder
                  .Build(single_class_build(), p.get(),
                         {SolutionFilter("First entry is class 0",
                                         [](const Solution& s) {
                                           return s.Id(0).Class(kClassInt) == 0;
                                         },
                                         {kClassInt})})
                  .ok());
  ActiveSet active_set_first_is_0 = first_is_0_builder.active_set(kClassInt);

  FilterToActiveSet first_is_1_builder(&entry_descriptor);
  ASSERT_TRUE(first_is_1_builder
                  .Build(single_class_build(), p.get(),
                         {SolutionFilter("First entry is class 1",
                                         [](const Solution& s) {
                                           return s.Id(0).Class(kClassInt) == 1;
                                         },
                                         {kClassInt})})
                  .ok());
  ActiveSet active_set_first_is_1 = first_is_1_builder.active_set(kClassInt);

  FilterToActiveSet first_is_2_builder(&entry_descriptor);
  ASSERT_TRUE(first_is_2_builder
                  .Build(single_class_build(), p.get(),
                         {SolutionFilter("First entry is class 2",
                                         [](const Solution& s) {
                                           return s.Id(0).Class(kClassInt) == 2;
                                         },
                                         {kClassInt})})
                  .ok());
  ActiveSet active_set_first_is_2 = first_is_2_builder.active_set(kClassInt);

  std::set<int> position_history;
  std::set<std::vector<int>> vector_history;
  for (auto it = p->begin().WithActiveSet(active_set_first_is_0);
       it != p->end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second) << it.position();
    EXPECT_TRUE(vector_history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(0));
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
  }

  for (auto it = p->begin().WithActiveSet(active_set_first_is_1);
       it != p->end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second) << it.position();
    EXPECT_TRUE(vector_history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(1));
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
  }

  for (auto it = p->begin().WithActiveSet(active_set_first_is_2);
       it != p->end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second) << it.position();
    EXPECT_TRUE(vector_history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(2));
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
  }

  EXPECT_THAT(position_history.size(), Eq(6));
  EXPECT_THAT(vector_history.size(), Eq(6));
}

TEST_P(SinglePermuterTest, ExistingSet) {
  static constexpr int kClassInt = 0;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(absl::make_unique<IntRangeDescriptor>(3),
                                   absl::make_unique<StringDescriptor>(
                                       std::vector<std::string>{"test class"}),
                                   std::move(class_descriptors));
  const Descriptor* class_descriptor =
      entry_descriptor.AllClassValues(kClassInt);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> p(
      MakeClassPermuter(class_descriptor, kClassInt));
  ASSERT_THAT(p->permutation_count(), 6);

  SolutionFilter first_is_0(
      "First entry is class 0",
      [](const Solution& s) { return s.Id(0).Class(kClassInt) == 0; },
      {kClassInt});

  SolutionFilter second_is_1(
      "Second entry is class 1",
      [](const Solution& s) { return s.Id(1).Class(kClassInt) == 1; },
      {kClassInt});

  LOG(INFO) << "Start: " << builder.active_set(kClassInt).DebugString();
  ASSERT_TRUE(builder.Build(single_class_build(), p.get(), {first_is_0}).ok());
  LOG(INFO) << "Add " << first_is_0.name() << ": "
            << builder.active_set(kClassInt).DebugString();
  ASSERT_TRUE(builder.Build(single_class_build(), p.get(), {second_is_1}).ok());
  LOG(INFO) << "Add " << second_is_1.name() << ": "
            << builder.active_set(kClassInt).DebugString();

  for (auto it = p->begin().WithActiveSet(builder.active_set(kClassInt));
       it != p->end(); ++it) {
    EXPECT_THAT((*it)[0], Eq(0));
    EXPECT_THAT((*it)[1], Eq(1));
  }
}

class PairPermuterTest
    : public ::testing::TestWithParam<FilterToActiveSet::PairClassImpl> {
 public:
  FilterToActiveSet::PairClassImpl pair_class_impl() { return GetParam(); }
};

INSTANTIATE_TEST_SUITE_P(
    Instantiation, PairPermuterTest,
    testing::Values(FilterToActiveSet::PairClassImpl::kPairSet,
                    FilterToActiveSet::PairClassImpl::kBackAndForth,
                    FilterToActiveSet::PairClassImpl::kPassThroughA));

TEST_P(PairPermuterTest, Simple) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  SolutionFilter c("a is 0 and b is 1 for id 0",
                   [](const Solution& s) {
                     return s.Id(0).Class(kClassIntA) == 0 &&
                            s.Id(0).Class(kClassIntB) == 1;
                   },
                   {kClassIntA, kClassIntB});

  ASSERT_TRUE(
      builder.Build(pair_class_impl(), permuter_a.get(), permuter_b.get(), {c})
          .ok());

  ActiveSet active_set_a = builder.active_set(kClassIntA);
  ActiveSet active_set_b = builder.active_set(kClassIntB);

  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a->begin(); it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b->begin(); it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++full_iteration_count;
      if (c(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a->begin().WithActiveSet(active_set_a);
       it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b->begin().WithActiveSet(active_set_b);
         it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++got_iteration_count;
      if (c(test_solution)) {
        ++got_found_count;
      }
    }
  }
  LOG(INFO) << "Iteration change " << full_iteration_count << " => "
            << got_iteration_count;
  EXPECT_THAT(got_iteration_count, Le(0.5 * full_iteration_count));
  EXPECT_THAT(got_found_count, Eq(expect_found_count));
}

TEST_P(PairPermuterTest, ExistingActiveSet) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  SolutionFilter a_filter(
      "a is 1 for id 1",
      [](const Solution& s) { return s.Id(1).Class(kClassIntA) == 1; },
      {kClassIntA});

  ASSERT_TRUE(builder.Build(permuter_a.get(), {a_filter}).ok());
  ActiveSet active_set_a_pre = builder.active_set(kClassIntA);

  SolutionFilter pair_filter("a is 0 and b is 1 for id 0",
                             [](const Solution& s) {
                               return s.Id(0).Class(kClassIntA) == 0 &&
                                      s.Id(0).Class(kClassIntB) == 1;
                             },
                             {kClassIntA, kClassIntB});

  ASSERT_TRUE(builder
                  .Build(pair_class_impl(), permuter_a.get(), permuter_b.get(),
                         {pair_filter})
                  .ok());

  ActiveSet active_set_a_post = builder.active_set(kClassIntA);
  ActiveSet active_set_b = builder.active_set(kClassIntB);

  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a->begin().WithActiveSet(active_set_a_pre);
       it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    EXPECT_TRUE(a_filter(test_solution));
    for (auto it_b = permuter_b->begin(); it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++full_iteration_count;
      if (pair_filter(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a->begin().WithActiveSet(active_set_a_post);
       it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    EXPECT_TRUE(a_filter(test_solution));
    for (auto it_b = permuter_b->begin().WithActiveSet(active_set_b);
         it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++got_iteration_count;
      if (pair_filter(test_solution)) {
        ++got_found_count;
      }
    }
  }
  LOG(INFO) << "Iteration change " << full_iteration_count << " => "
            << got_iteration_count;
  EXPECT_THAT(got_iteration_count, Le(0.5 * full_iteration_count));
  EXPECT_THAT(got_found_count, Eq(expect_found_count));
}

TEST_P(PairPermuterTest, ExistingActiveSetForB) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  SolutionFilter b_filter(
      "b is 2 for id 1",
      [](const Solution& s) { return s.Id(1).Class(kClassIntB) == 2; },
      {kClassIntB});

  ASSERT_TRUE(builder.Build(permuter_b.get(), {b_filter}).ok());
  ActiveSet active_set_b_pre = builder.active_set(kClassIntB);

  SolutionFilter pair_filter("a is 0 and b is 1 for id 0",
                             [](const Solution& s) {
                               return s.Id(0).Class(kClassIntA) == 0 &&
                                      s.Id(0).Class(kClassIntB) == 1;
                             },
                             {kClassIntA, kClassIntB});

  ASSERT_TRUE(builder
                  .Build(pair_class_impl(), permuter_a.get(), permuter_b.get(),
                         {pair_filter})
                  .ok());

  ActiveSet active_set_a = builder.active_set(kClassIntA);
  ActiveSet active_set_b_post = builder.active_set(kClassIntB);

  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a->begin(); it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b->begin().WithActiveSet(active_set_b_pre);
         it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      EXPECT_TRUE(b_filter(test_solution));
      ++full_iteration_count;
      if (pair_filter(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a->begin().WithActiveSet(active_set_a);
       it_a != permuter_a->end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b->begin().WithActiveSet(active_set_b_post);
         it_b != permuter_b->end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      EXPECT_TRUE(b_filter(test_solution));
      ++got_iteration_count;
      if (pair_filter(test_solution)) {
        ++got_found_count;
      }
    }
  }
  LOG(INFO) << "Iteration change " << full_iteration_count << " => "
            << got_iteration_count;
  EXPECT_THAT(got_iteration_count, Le(0.5 * full_iteration_count));
  EXPECT_THAT(got_found_count, Eq(expect_found_count));
}

TEST_P(PairPermuterTest, MakePairs) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  int i = 0;
  std::vector<int> a0_is_0;
  int a0_is_not_0 = -1;
  for (absl::Span<const int> a_vals : *permuter_a) {
    if (a_vals[0] == 0)
      a0_is_0.push_back(i);
    else
      a0_is_not_0 = i;
    ++i;
  }
  ASSERT_NE(a0_is_not_0, -1);
  ASSERT_EQ(a0_is_0.size(), 2);
  i = 0;
  std::vector<int> b0_is_1;
  int b0_is_not_1 = -1;
  for (absl::Span<const int> b_vals : *permuter_b) {
    if (b_vals[0] == 1)
      b0_is_1.push_back(i);
    else
      b0_is_not_1 = i;
    ++i;
  }
  ASSERT_NE(b0_is_not_1, -1);
  ASSERT_EQ(b0_is_1.size(), 2);

  SolutionFilter c("a is 0 and b is 1 for id 0",
                   [](const Solution& s) {
                     return s.Id(0).Class(kClassIntA) == 0 &&
                            s.Id(0).Class(kClassIntB) == 1;
                   },
                   {kClassIntA, kClassIntB});

  ASSERT_TRUE(builder
                  .Build(pair_class_impl(), permuter_a.get(), permuter_b.get(),
                         {c}, FilterToActiveSet::PairClassMode::kMakePairs)
                  .ok());

  EXPECT_THAT(
      builder.active_set_pair(kClassIntA, /*a_val=*/a0_is_not_0, kClassIntB)
          .EnabledValues(),
      IsEmpty());
  EXPECT_THAT(
      builder.active_set_pair(kClassIntB, /*a_val=*/b0_is_not_1, kClassIntA)
          .EnabledValues(),
      IsEmpty());

  EXPECT_THAT(
      builder.active_set_pair(kClassIntA, /*a_val=*/a0_is_0[0], kClassIntB)
          .EnabledValues(),
      ElementsAreArray(b0_is_1));
  EXPECT_THAT(
      builder.active_set_pair(kClassIntB, /*a_val=*/b0_is_1[0], kClassIntA)
          .EnabledValues(),
      ElementsAreArray(a0_is_0));
}

TEST_P(PairPermuterTest, MakePairsEntryPredicate) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  int i = 0;
  std::vector<int> a0_is_0;
  int a0_is_not_0 = -1;
  for (absl::Span<const int> a_vals : *permuter_a) {
    if (a_vals[0] == 0)
      a0_is_0.push_back(i);
    else
      a0_is_not_0 = i;
    ++i;
  }
  ASSERT_NE(a0_is_not_0, -1);
  ASSERT_EQ(a0_is_0.size(), 2);
  i = 0;
  std::vector<int> b0_is_1;
  int b0_is_not_1 = -1;
  for (absl::Span<const int> b_vals : *permuter_b) {
    if (b_vals[0] == 1)
      b0_is_1.push_back(i);
    else
      b0_is_not_1 = i;
    ++i;
  }
  ASSERT_NE(b0_is_not_1, -1);
  ASSERT_EQ(b0_is_1.size(), 2);

  SolutionFilter c(
      "a is 0 and b is 1 for id 0",
      [](const Entry& e) {
        return e.Class(kClassIntA) == 0 && e.Class(kClassIntB) == 1;
      },
      {kClassIntA, kClassIntB}, /*entry_id=*/0);

  ASSERT_TRUE(builder
                  .Build(pair_class_impl(), permuter_a.get(), permuter_b.get(),
                         {c}, FilterToActiveSet::PairClassMode::kMakePairs)
                  .ok());

  EXPECT_THAT(
      builder.active_set_pair(kClassIntA, /*a_val=*/a0_is_not_0, kClassIntB)
          .EnabledValues(),
      IsEmpty());
  EXPECT_THAT(
      builder.active_set_pair(kClassIntB, /*a_val=*/b0_is_not_1, kClassIntA)
          .EnabledValues(),
      IsEmpty());

  EXPECT_THAT(
      builder.active_set_pair(kClassIntA, /*a_val=*/a0_is_0[0], kClassIntB)
          .EnabledValues(),
      ElementsAreArray(b0_is_1));
  EXPECT_THAT(
      builder.active_set_pair(kClassIntB, /*a_val=*/b0_is_1[0], kClassIntA)
          .EnabledValues(),
      ElementsAreArray(a0_is_0));
}

TEST_P(PairPermuterTest, MakePairsOrFilter) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);

  auto val_0_predicate = [](int a_val, int b_val) {
    return (a_val == 0 && b_val == 1) || (a_val == 1 && b_val == 0);
  };

  absl::flat_hash_map<int, absl::flat_hash_set<int>> a_to_b_vals;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> b_to_a_vals;
  int a_0_is_2 = -1;
  int b_0_is_2 = -1;
  int index_a = 0;
  for (auto it_a = permuter_a->begin(); it_a != permuter_a->end();
       ++index_a, ++it_a) {
    absl::Span<const int> a_vals = *it_a;
    const int a_0_val = a_vals[0];
    if (a_0_val == 2) a_0_is_2 = index_a;
    int index_b = 0;
    for (auto it_b = permuter_b->begin(); it_b != permuter_b->end();
         ++index_b, ++it_b) {
      absl::Span<const int> b_vals = *it_b;
      const int b_0_val = b_vals[0];
      if (b_0_val == 2) b_0_is_2 = index_b;
      if (val_0_predicate(a_0_val, b_0_val)) {
        a_to_b_vals[a_0_val].insert(index_b);
        b_to_a_vals[b_0_val].insert(index_a);
      }
    }
  }

  SolutionFilter c("vals[0].{a,b} IN ((3,4), (4, 3))",
                   [val_0_predicate](const Solution& s) {
                     return val_0_predicate(s.Id(0).Class(kClassIntA),
                                            s.Id(0).Class(kClassIntB));
                   },
                   {kClassIntA, kClassIntB});

  ASSERT_TRUE(builder
                  .Build(pair_class_impl(), permuter_a.get(), permuter_b.get(),
                         {c}, FilterToActiveSet::PairClassMode::kMakePairs)
                  .ok());

  EXPECT_THAT(
      builder.active_set_pair(kClassIntA, /*a_val=*/a_0_is_2, kClassIntB)
          .EnabledValues(),
      IsEmpty());
  EXPECT_THAT(
      builder.active_set_pair(kClassIntB, /*a_val=*/b_0_is_2, kClassIntA)
          .EnabledValues(),
      IsEmpty());

  EXPECT_THAT(builder
                  .active_set_pair(
                      kClassIntA, /*a_val=*/*b_to_a_vals[1].begin(), kClassIntB)
                  .EnabledValues(),
              UnorderedElementsAreArray(a_to_b_vals[0]));
  EXPECT_THAT(builder
                  .active_set_pair(
                      kClassIntB, /*a_val=*/*a_to_b_vals[0].begin(), kClassIntA)
                  .EnabledValues(),
              UnorderedElementsAreArray(b_to_a_vals[1]));
}

TEST_P(PairPermuterTest, MakePairsCycle) {
  static constexpr int kClassIntA = 0;
  static constexpr int kClassIntB = 1;
  static constexpr int kClassIntC = 2;
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor entry_descriptor(
      absl::make_unique<IntRangeDescriptor>(3),
      absl::make_unique<StringDescriptor>(
          std::vector<std::string>{"class a", "class b", "class c"}),
      std::move(class_descriptors));
  const Descriptor* class_descriptor_a =
      entry_descriptor.AllClassValues(kClassIntA);
  const Descriptor* class_descriptor_b =
      entry_descriptor.AllClassValues(kClassIntB);
  const Descriptor* class_descriptor_c =
      entry_descriptor.AllClassValues(kClassIntC);

  FilterToActiveSet builder(&entry_descriptor);

  std::unique_ptr<ClassPermuter> permuter_a(
      MakeClassPermuter(class_descriptor_a, kClassIntA));
  std::unique_ptr<ClassPermuter> permuter_b(
      MakeClassPermuter(class_descriptor_b, kClassIntB));
  std::unique_ptr<ClassPermuter> permuter_c(
      MakeClassPermuter(class_descriptor_c, kClassIntC));
  ASSERT_THAT(permuter_a->permutation_count(), 6);
  ASSERT_THAT(permuter_b->permutation_count(), 6);
  ASSERT_THAT(permuter_c->permutation_count(), 6);

  SolutionFilter a_b("a is 0 and b is 1 for id 0",
                     [](const Solution& s) {
                       return s.Id(0).Class(kClassIntA) == 0 &&
                              s.Id(0).Class(kClassIntB) == 1;
                     },
                     {kClassIntA, kClassIntB});

  SolutionFilter b_c("b is 2 and c is 0 for id 1",
                     [](const Solution& s) {
                       return s.Id(1).Class(kClassIntB) == 2 &&
                              s.Id(1).Class(kClassIntC) == 0;
                     },
                     {kClassIntB, kClassIntC});

  SolutionFilter c_a("c is 1 and a is 2 for id 2",
                     [](const Solution& s) {
                       return s.Id(2).Class(kClassIntC) == 1 &&
                              s.Id(2).Class(kClassIntA) == 2;
                     },
                     {kClassIntC, kClassIntA});

  for (const auto& loop : {1, 2, 3}) {
    std::ignore = loop;
    ASSERT_TRUE(builder
                    .Build(pair_class_impl(), permuter_a.get(),
                           permuter_b.get(), {a_b},
                           FilterToActiveSet::PairClassMode::kMakePairs)
                    .ok());

    ASSERT_TRUE(builder
                    .Build(pair_class_impl(), permuter_b.get(),
                           permuter_c.get(), {b_c},
                           FilterToActiveSet::PairClassMode::kMakePairs)
                    .ok());

    ASSERT_TRUE(builder
                    .Build(pair_class_impl(), permuter_c.get(),
                           permuter_a.get(), {c_a},
                           FilterToActiveSet::PairClassMode::kMakePairs)
                    .ok());
  }

  EXPECT_EQ(builder.active_set(kClassIntA).matches(), 1) << *permuter_a;
  EXPECT_EQ(builder.active_set(kClassIntB).matches(), 1) << *permuter_b;
  EXPECT_EQ(builder.active_set(kClassIntC).matches(), 1) << *permuter_c;
}

}  // namespace puzzle
