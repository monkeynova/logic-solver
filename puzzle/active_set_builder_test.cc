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
using ::testing::IsEmpty;
using ::testing::ElementsAreArray;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

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

TEST_P(PairPermuterTest, MakePairs) {
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

  int i = 0;
  std::vector<int> a0_is_3;
  for (const std::vector<int>& a_vals : permuter_a) {
    if (a_vals[0] == 3) a0_is_3.push_back(i);
    ++i;
  }
  ASSERT_EQ(a0_is_3.size(), 2);
  i = 0;
  std::vector<int> b0_is_4;
  for (const std::vector<int>& b_vals : permuter_b) {
    if (b_vals[0] == 4) b0_is_4.push_back(i);
    ++i;
  }
  ASSERT_EQ(b0_is_4.size(), 2);

  Solution::Cropper c("a is 3 and b is 4 for id 0",
                      [](const Solution& s) {
                        return s.Id(0).Class(kClassIntA) == 3 &&
                          s.Id(0).Class(kClassIntB) == 4;
                      },
                      {kClassIntA, kClassIntB});

  builder.Build(pair_class_impl(), permuter_a, permuter_b, {c},
		ActiveSetBuilder::PairClassMode::kMakePairs);

  EXPECT_THAT(builder.active_set_pair(kClassIntA, /*a_val=*/5, kClassIntB)
	          .EnabledValues(),
	      IsEmpty());
  EXPECT_THAT(builder.active_set_pair(kClassIntB, /*a_val=*/3, kClassIntA)
	          .EnabledValues(),
	      IsEmpty());

  EXPECT_THAT(builder.active_set_pair(kClassIntA, /*a_val=*/3, kClassIntB)
	          .EnabledValues(),
	      ElementsAreArray(b0_is_4));
  EXPECT_THAT(builder.active_set_pair(kClassIntB, /*a_val=*/4, kClassIntA)
	          .EnabledValues(),
	      ElementsAreArray(a0_is_3));
}

TEST_P(PairPermuterTest, MakePairsOrFilter) {
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

  auto val_0_predicate =
    [](int a_val, int b_val) {
      return (a_val == 3 && b_val == 4) || (a_val == 4 && b_val == 3);
    }; 
  
  absl::flat_hash_map<int, absl::flat_hash_set<int>> a_to_b_vals;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> b_to_a_vals;
  int index_a = 0;
  for (auto it_a = permuter_a.begin(); it_a != permuter_a.end();
       ++index_a, ++it_a) {
    const std::vector<int>& a_vals = *it_a;
    const int a_0_val = a_vals[0];
    int index_b = 0;
    for (auto it_b = permuter_b.begin(); it_b != permuter_b.end();
	 ++index_b, ++it_b) {
      const std::vector<int>& b_vals = *it_b;
      const int b_0_val = b_vals[0];
      if (val_0_predicate(a_0_val, b_0_val)) {
	a_to_b_vals[a_0_val].insert(index_b);
	b_to_a_vals[b_0_val].insert(index_a);
      }
    }
  }

  Solution::Cropper c("vals[0].{a,b} IN ((3,4), (4, 3))",
                      [val_0_predicate](const Solution& s) {
                        return val_0_predicate(s.Id(0).Class(kClassIntA),
					       s.Id(0).Class(kClassIntB));
                      },
                      {kClassIntA, kClassIntB});

  builder.Build(pair_class_impl(), permuter_a, permuter_b, {c},
		ActiveSetBuilder::PairClassMode::kMakePairs);

  EXPECT_THAT(builder.active_set_pair(kClassIntA, /*a_val=*/5, kClassIntB)
	          .EnabledValues(),
	      IsEmpty());
  EXPECT_THAT(builder.active_set_pair(kClassIntB, /*a_val=*/5, kClassIntA)
	          .EnabledValues(),
	      IsEmpty());

  EXPECT_THAT(builder.active_set_pair(kClassIntA, /*a_val=*/3, kClassIntB)
	          .EnabledValues(),
	      UnorderedElementsAreArray(a_to_b_vals[3]));
  EXPECT_THAT(builder.active_set_pair(kClassIntB, /*a_val=*/4, kClassIntA)
	          .EnabledValues(),
	      UnorderedElementsAreArray(b_to_a_vals[4]));
}


TEST_P(PairPermuterTest, MakePairsCycle) {
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassIntA = 0;
  const int kClassIntB = 1;
  const int kClassIntC = 2;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor_a(3, 5);
  entry_descriptor.SetClass(kClassIntA, "class a", &class_descriptor_a);
  IntRangeDescriptor class_descriptor_b(3, 5);
  entry_descriptor.SetClass(kClassIntB, "class b", &class_descriptor_b);
  IntRangeDescriptor class_descriptor_c(3, 5);
  entry_descriptor.SetClass(kClassIntC, "class c", &class_descriptor_c);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter permuter_a(&class_descriptor_a, kClassIntA);
  ClassPermuter permuter_b(&class_descriptor_b, kClassIntB);
  ClassPermuter permuter_c(&class_descriptor_c, kClassIntC);
  ASSERT_THAT(permuter_a.permutation_count(), 6);
  ASSERT_THAT(permuter_b.permutation_count(), 6);
  ASSERT_THAT(permuter_c.permutation_count(), 6);

  
  Solution::Cropper a_b("a is 3 and b is 4 for id 0",
			[](const Solution& s) {
			  return s.Id(0).Class(kClassIntA) == 3 &&
			    s.Id(0).Class(kClassIntB) == 4;
			},
			{kClassIntA, kClassIntB});

  Solution::Cropper b_c("b is 5 and c is 3 for id 1",
			[](const Solution& s) {
			  return s.Id(1).Class(kClassIntB) == 5 &&
			    s.Id(1).Class(kClassIntC) == 3;
			},
			{kClassIntB, kClassIntC});

  Solution::Cropper c_a("c is 4 and a is 5 for id 2",
			[](const Solution& s) {
			  return s.Id(2).Class(kClassIntC) == 4 &&
			    s.Id(2).Class(kClassIntA) == 5;
			},
			{kClassIntC, kClassIntA});

  
  for (const auto& loop : {1, 2, 3}) {
    std::ignore = loop;
    builder.Build(pair_class_impl(), permuter_a, permuter_b, {a_b},
		  ActiveSetBuilder::PairClassMode::kMakePairs);

    permuter_a.set_active_set(builder.active_set(kClassIntA));
    permuter_b.set_active_set(builder.active_set(kClassIntB));
    
    builder.Build(pair_class_impl(), permuter_b, permuter_c, {b_c},
		  ActiveSetBuilder::PairClassMode::kMakePairs);
    
    permuter_b.set_active_set(builder.active_set(kClassIntB));
    permuter_c.set_active_set(builder.active_set(kClassIntC));
    
    builder.Build(pair_class_impl(), permuter_c, permuter_a, {c_a},
		  ActiveSetBuilder::PairClassMode::kMakePairs);
    
    permuter_c.set_active_set(builder.active_set(kClassIntC));
    permuter_a.set_active_set(builder.active_set(kClassIntA));
  }

  EXPECT_EQ(builder.active_set(kClassIntA).matches(), 1)
    << permuter_a;
  EXPECT_EQ(builder.active_set(kClassIntB).matches(), 1)
    << permuter_b;
  EXPECT_EQ(builder.active_set(kClassIntC).matches(), 1)
    << permuter_c;
}

}  // namespace puzzle
