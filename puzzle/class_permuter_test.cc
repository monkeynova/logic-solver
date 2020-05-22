#include "puzzle/class_permuter.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set.h"
#include "puzzle/class_permuter_factorial_radix.h"
#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

using ::testing::UnorderedElementsAre;

namespace puzzle {

template <typename T>
class ClassPermuterTest : public ::testing::Test {};

using ClassPermuterTypes =
    ::testing::Types<ClassPermuterSteinhausJohnsonTrotter,
                     ClassPermuterFactorialRadix,
                     ClassPermuterFactorialRadixDeleteTracking>;
TYPED_TEST_SUITE(ClassPermuterTest, ClassPermuterTypes);

TYPED_TEST(ClassPermuterTest, ThreeElements) {
  IntRangeDescriptor d(3, 5);
  TypeParam p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 6);
}

TYPED_TEST(ClassPermuterTest, FiveElements) {
  IntRangeDescriptor d(3, 7);
  TypeParam p(&d);
  EXPECT_THAT(p.permutation_count(), 120);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5, 6, 7));
    ++position;
  }
  EXPECT_THAT(position, 120);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkips) {
  IntRangeDescriptor d(3, 5);
  TypeParam p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  ActiveSet active_set_first;
  ActiveSet active_set_last;
  for (int i = 0; i < 6; ++i) {
    active_set_first.Add(i < 3);
    active_set_last.Add(i >= 3);
  }
  active_set_first.DoneAdding();
  active_set_last.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  p.set_active_set(std::move(active_set_first));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  p.set_active_set(std::move(active_set_last));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position + 3);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkipsShredded) {
  IntRangeDescriptor d(3, 5);
  TypeParam p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  ActiveSet active_set_odd;
  ActiveSet active_set_even;
  for (int i = 0; i < 6; ++i) {
    active_set_odd.Add(i & 1);
    active_set_even.Add(!(i & 1));
  }
  active_set_odd.DoneAdding();
  active_set_even.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  p.set_active_set(std::move(active_set_even));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  p.set_active_set(std::move(active_set_odd));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkipsShreddedByBeginArg) {
  IntRangeDescriptor d(3, 5);
  TypeParam p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  ActiveSet active_set_odd;
  ActiveSet active_set_even;
  for (int i = 0; i < 6; ++i) {
    active_set_odd.Add(i & 1);
    active_set_even.Add(!(i & 1));
  }
  active_set_odd.DoneAdding();
  active_set_even.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin(active_set_even); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  for (auto it = p.begin(active_set_odd); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ValueSkip) {
  constexpr int permuter_size = 9;
  IntRangeDescriptor d(1, permuter_size);
  TypeParam p(&d);
  const int permutations = p.permutation_count();

  for (int value_index = 0; value_index < permuter_size; ++value_index) {
    int loop_count = 0;
    int last_val = -1;
    for (auto it = p.begin(); it != p.end();
         it += {.value_index = value_index}) {
      EXPECT_NE(last_val, (*it)[value_index]);
      last_val = (*it)[value_index];
      ++loop_count;
    }
    EXPECT_LE(loop_count, permutations) << "Iteration: " << value_index;
    LOG(INFO) << "Reduction [" << value_index << "]: " << permutations << " => "
              << loop_count;
  }
}

TYPED_TEST(ClassPermuterTest, ValueSkipBadId) {
  constexpr int permuter_size = 9;
  IntRangeDescriptor d(1, permuter_size);
  TypeParam p(&d);
  const int permutations = p.permutation_count();

  int loop_count = 0;
  for (auto it = p.begin(); it != p.end();
       it += {.value_index = Entry::kBadId}) {
    ++loop_count;
  }
  EXPECT_EQ(loop_count, permutations);
}

TYPED_TEST(ClassPermuterTest, ValueSkipWithActiveSet) {
  constexpr int permuter_size = 4;
  IntRangeDescriptor d(1, permuter_size);
  TypeParam p(&d);

  ActiveSet only_three_in_position_one;
  for (const auto& permutation : p) {
    only_three_in_position_one.Add(permutation[1] == 3);
  }
  only_three_in_position_one.DoneAdding();
  EXPECT_EQ(only_three_in_position_one.matches(), 6);

  p.set_active_set(only_three_in_position_one);
  int loop_count = 0;
  for (const auto& permutation : p) {
    std::ignore = permutation;
    ++loop_count;
  }
  EXPECT_EQ(loop_count, only_three_in_position_one.matches());

  loop_count = 0;
  for (auto it = p.begin(); it != p.end(); it += {.value_index = 1}) {
    EXPECT_EQ((*it)[1], 3);
    ++loop_count;
  }
  EXPECT_EQ(loop_count, 1);
}

}  // namespace puzzle
