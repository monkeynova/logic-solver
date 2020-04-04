#include "puzzle/active_set_builder.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"

using ::testing::Eq;
using ::testing::Le;
using ::testing::UnorderedElementsAre;

namespace puzzle {

class SinglePermuterTest
  : public ::testing::TestWithParam<ActiveSetBuilder::SingleClassBuild> {
 public:
  ActiveSetBuilder::SingleClassBuild single_class_build() {
    return GetParam();
  }
};

INSTANTIATE_TEST_SUITE_P(
    Instantiation,
    SinglePermuterTest,
    testing::Values(ActiveSetBuilder::SingleClassBuild::kPassThrough,
                    ActiveSetBuilder::SingleClassBuild::kPositionSet));

TEST_P(SinglePermuterTest, Simple) {
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassInt = 0;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor(3, 5);
  entry_descriptor.SetClass(kClassInt, "test class", &class_descriptor);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter p(&class_descriptor, kClassInt);
  ASSERT_THAT(p.permutation_count(), 6);

  builder.Build(
      single_class_build(), p,
      {Solution::Cropper("First entry is class 3",
                         [](const Solution& s) {
                           return s.Id(0).Class(kClassInt) == 3;
                         },
                         {kClassInt})});
  ActiveSet active_set_first_is_3 = builder.active_set(kClassInt);

  builder.Build(
      single_class_build(), p,
      {Solution::Cropper("First entry is class 4",
                         [](const Solution& s) {
                           return s.Id(0).Class(kClassInt) == 4;
                         },
                         {kClassInt})});
  ActiveSet active_set_first_is_4 = builder.active_set(kClassInt);

  builder.Build(
      single_class_build(), p,
      {Solution::Cropper("First entry is class 5",
                         [](const Solution& s) {
                           return s.Id(0).Class(kClassInt) == 5;
                         },
                         {kClassInt})});
  ActiveSet active_set_first_is_5 = builder.active_set(kClassInt);

  std::set<int> position_history;
  std::set<std::vector<int>> vector_history;
  p.set_active_set(std::move(active_set_first_is_3));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second)
      << it.position();
    EXPECT_TRUE(vector_history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(3));
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
  }

  p.set_active_set(std::move(active_set_first_is_4));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second)
      << it.position();
    EXPECT_TRUE(vector_history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(4));
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
  }

  p.set_active_set(std::move(active_set_first_is_5));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_TRUE(position_history.insert(it.position()).second)
      << it.position();
    EXPECT_TRUE(vector_history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT((*it)[0], Eq(5));
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
  }

  EXPECT_THAT(position_history.size(), Eq(6));
  EXPECT_THAT(vector_history.size(), Eq(6));
}

TEST_P(SinglePermuterTest, ExistingSet) {
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassInt = 0;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor(3, 5);
  entry_descriptor.SetClass(kClassInt, "test class", &class_descriptor);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter p(&class_descriptor, kClassInt);
  ASSERT_THAT(p.permutation_count(), 6);

  Solution::Cropper first_is_3("First entry is class 3",
                               [](const Solution& s) {
                                 return s.Id(0).Class(kClassInt) == 3;
                               },
                               {kClassInt});

  Solution::Cropper second_is_4("Second entry is class 4",
                                [](const Solution& s) {
                                  return s.Id(1).Class(kClassInt) == 4;
                                },
                                {kClassInt});

  LOG(INFO) << "Start: " << p.active_set().DebugString();
  builder.Build(single_class_build(), p, {first_is_3});
  p.set_active_set(builder.active_set(kClassInt));
  LOG(INFO) << "Add " << first_is_3.name << ": "
            << p.active_set().DebugString();
  builder.Build(single_class_build(), p, {second_is_4});
  p.set_active_set(builder.active_set(kClassInt));
  LOG(INFO) << "Add " << second_is_4.name << ": "
            << p.active_set().DebugString();

  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT((*it)[0], Eq(3));
    EXPECT_THAT((*it)[1], Eq(4));
  }
}

class PairPermuterTest
  : public ::testing::TestWithParam<ActiveSetBuilder::PairClassImpl> {
 public:
  ActiveSetBuilder::PairClassImpl pair_class_impl() {
    return GetParam();
  }
};

INSTANTIATE_TEST_SUITE_P(
    Instantiation,
    PairPermuterTest,
    testing::Values(ActiveSetBuilder::PairClassImpl::kPairSet,
                    ActiveSetBuilder::PairClassImpl::kBackAndForth,
                    ActiveSetBuilder::PairClassImpl::kPassThroughA));

TEST_P(PairPermuterTest, Simple) {
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassIntA = 0;
  const int kClassIntB = 1;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor_a(3, 5);
  entry_descriptor.SetClass(kClassIntA, "class a", &class_descriptor_a);
  IntRangeDescriptor class_descriptor_b(3, 5);
  entry_descriptor.SetClass(kClassIntB, "class b", &class_descriptor_b);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter permuter_a(&class_descriptor_a, kClassIntA);
  ClassPermuter permuter_b(&class_descriptor_b, kClassIntB);
  ASSERT_THAT(permuter_a.permutation_count(), 6);
  ASSERT_THAT(permuter_b.permutation_count(), 6);

  Solution::Cropper c("a is 3 and b is 4 for id 0",
                      [](const Solution& s) {
                        return s.Id(0).Class(kClassIntA) == 3 &&
                          s.Id(0).Class(kClassIntB) == 4;
                      },
                      {kClassIntA, kClassIntB});

  builder.Build(pair_class_impl(), permuter_a, permuter_b, {c});

  ActiveSet active_set_a = builder.active_set(kClassIntA);
  ActiveSet active_set_b = builder.active_set(kClassIntB);


  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++full_iteration_count;
      if (c.p(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  permuter_a.set_active_set(active_set_a);
  permuter_b.set_active_set(active_set_b);

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++got_iteration_count;
      if (c.p(test_solution)) {
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
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassIntA = 0;
  const int kClassIntB = 1;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor_a(3, 5);
  entry_descriptor.SetClass(kClassIntA, "class a", &class_descriptor_a);
  IntRangeDescriptor class_descriptor_b(3, 5);
  entry_descriptor.SetClass(kClassIntB, "class b", &class_descriptor_b);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter permuter_a(&class_descriptor_a, kClassIntA);
  ClassPermuter permuter_b(&class_descriptor_b, kClassIntB);
  ASSERT_THAT(permuter_a.permutation_count(), 6);
  ASSERT_THAT(permuter_b.permutation_count(), 6);

  Solution::Cropper a_cropper("a is 4 for id 1",
                              [](const Solution& s) {
                                return s.Id(1).Class(kClassIntA) == 4;
                              },
                              {kClassIntA});

  builder.Build(permuter_a, {a_cropper});
  ActiveSet active_set_a_pre = builder.active_set(kClassIntA);
  permuter_a.set_active_set(active_set_a_pre);

  Solution::Cropper pair_cropper("a is 3 and b is 4 for id 0",
                                 [](const Solution& s) {
                                   return s.Id(0).Class(kClassIntA) == 3 &&
                                     s.Id(0).Class(kClassIntB) == 4;
                                 },
                                 {kClassIntA, kClassIntB});

  builder.Build(pair_class_impl(), permuter_a, permuter_b, {pair_cropper});

  ActiveSet active_set_a_post = builder.active_set(kClassIntA);
  ActiveSet active_set_b = builder.active_set(kClassIntB);

  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    EXPECT_TRUE(a_cropper.p(test_solution));
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++full_iteration_count;
      if (pair_cropper.p(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  permuter_a.set_active_set(active_set_a_post);
  permuter_b.set_active_set(active_set_b);

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    EXPECT_TRUE(a_cropper.p(test_solution));
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      ++got_iteration_count;
      if (pair_cropper.p(test_solution)) {
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
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassIntA = 0;
  const int kClassIntB = 1;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor_a(3, 5);
  entry_descriptor.SetClass(kClassIntA, "class a", &class_descriptor_a);
  IntRangeDescriptor class_descriptor_b(3, 5);
  entry_descriptor.SetClass(kClassIntB, "class b", &class_descriptor_b);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter permuter_a(&class_descriptor_a, kClassIntA);
  ClassPermuter permuter_b(&class_descriptor_b, kClassIntB);
  ASSERT_THAT(permuter_a.permutation_count(), 6);
  ASSERT_THAT(permuter_b.permutation_count(), 6);

  Solution::Cropper b_cropper("b is 4 for id 1",
                              [](const Solution& s) {
                                return s.Id(1).Class(kClassIntB) == 5;
                              },
                              {kClassIntB});

  builder.Build(permuter_b, {b_cropper});
  ActiveSet active_set_b_pre = builder.active_set(kClassIntB);
  permuter_b.set_active_set(active_set_b_pre);

  Solution::Cropper pair_cropper("a is 3 and b is 4 for id 0",
                                 [](const Solution& s) {
                                   return s.Id(0).Class(kClassIntA) == 3 &&
                                     s.Id(0).Class(kClassIntB) == 4;
                                 },
                                 {kClassIntA, kClassIntB});

  builder.Build(pair_class_impl(), permuter_a, permuter_b, {pair_cropper});

  ActiveSet active_set_a = builder.active_set(kClassIntA);
  ActiveSet active_set_b_post = builder.active_set(kClassIntB);


  MutableSolution mutable_solution(&entry_descriptor);
  Solution test_solution = mutable_solution.TestableSolution();

  int expect_found_count = 0;
  int full_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      EXPECT_TRUE(b_cropper.p(test_solution));
      ++full_iteration_count;
      if (pair_cropper.p(test_solution)) {
        ++expect_found_count;
      }
    }
  }

  permuter_a.set_active_set(active_set_a);
  permuter_b.set_active_set(active_set_b_post);

  int got_found_count = 0;
  int got_iteration_count = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end(); ++it_a) {
    mutable_solution.SetClass(it_a);
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end(); ++it_b) {
      mutable_solution.SetClass(it_b);
      EXPECT_TRUE(b_cropper.p(test_solution));
      ++got_iteration_count;
      if (pair_cropper.p(test_solution)) {
        ++got_found_count;
      }
    }
  }
  LOG(INFO) << "Iteration change " << full_iteration_count << " => "
            << got_iteration_count;
  EXPECT_THAT(got_iteration_count, Le(0.5 * full_iteration_count));
  EXPECT_THAT(got_found_count, Eq(expect_found_count));
}

}  // namespace puzzle
