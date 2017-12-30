#include "puzzle/active_set_builder.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"

using ::testing::Eq;
using ::testing::Lt;
using ::testing::UnorderedElementsAre;

namespace puzzle {

TEST(ActiveSetBuilderTest, SingleClass) {
  IntRangeDescriptor id_descriptor(0, 2);
  EntryDescriptor entry_descriptor;
  const int kClassInt = 0;
  entry_descriptor.SetIds(&id_descriptor);
  IntRangeDescriptor class_descriptor(3, 5);
  entry_descriptor.SetClass(kClassInt, "test class", &class_descriptor);

  ActiveSetBuilder builder(&entry_descriptor);

  ClassPermuter p(&class_descriptor, kClassInt);
  ASSERT_THAT(p.permutation_count(), 6);
  
  ActiveSet active_set_first_is_3 = builder.Build(
      p,
      {Solution::Cropper("First entry is class 3",
			 [](const Solution& s) {
			   return s.Id(0).Class(kClassInt) == 3;
			 },
			 {kClassInt})});

  ActiveSet active_set_first_is_4 = builder.Build(
      p,
      {Solution::Cropper("First entry is class 4",
			 [](const Solution& s) {
			   return s.Id(0).Class(kClassInt) == 4;
			 },
			 {kClassInt})});

  ActiveSet active_set_first_is_5 = builder.Build(
      p,
      {Solution::Cropper("First entry is class 5",
			 [](const Solution& s) {
			   return s.Id(0).Class(kClassInt) == 5;
			 },
			 {kClassInt})});
  
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

  TEST(ActiveSetBuilderTest, MultiClass) {
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

  ActiveSet active_set_a;
  ActiveSet active_set_b;

  Solution::Cropper c("a is 3 and b is 4 for id 0",
		      [](const Solution& s) {
			return s.Id(0).Class(kClassIntA) == 3 &&
			  s.Id(0).Class(kClassIntB) == 4;
		      },
		      {kClassIntA, kClassIntB});

  builder.Build(permuter_a, permuter_b, {c}, &active_set_a, &active_set_b);

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
  EXPECT_THAT(got_iteration_count, Lt(0.5 * full_iteration_count));
  EXPECT_THAT(got_found_count, Eq(expect_found_count));
}

}  // namespace puzzle
