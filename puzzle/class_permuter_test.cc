#include "puzzle/class_permuter.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set/active_set.h"
#include "puzzle/class_permuter_factorial_radix.h"
#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

using ::testing::UnorderedElementsAre;

namespace puzzle {

template <typename T>
class ClassPermuterTest : public ::testing::Test {};

using ClassPermuterTypes =
    ::testing::Types<MakeClassPermuterSteinhausJohnsonTrotter,
                     MakeClassPermuterFactorialRadix,
                     MakeClassPermuterFactorialRadixDeleteTracking>;
TYPED_TEST_SUITE(ClassPermuterTest, ClassPermuterTypes);

TYPED_TEST(ClassPermuterTest, ThreeElements) {
  constexpr int kPermuterSize = 3;
  auto p = TypeParam()(kPermuterSize);
  EXPECT_THAT(p->permutation_count(), 6);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p->begin(); it != p->end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 6);
}

TYPED_TEST(ClassPermuterTest, FiveElements) {
  constexpr int kPermuterSize = 5;
  auto p = TypeParam()(kPermuterSize);
  EXPECT_THAT(p->permutation_count(), 120);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p->begin(); it != p->end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2, 3, 4));
    ++position;
  }
  EXPECT_THAT(position, 120);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkips) {
  constexpr int kPermuterSize = 3;
  auto p = TypeParam()(kPermuterSize);
  EXPECT_THAT(p->permutation_count(), 6);

  ActiveSet::Builder builder_first(6);
  ActiveSet::Builder builder_last(6);
  for (int i = 0; i < 6; ++i) {
    builder_first.Add(i < 3);
    builder_last.Add(i >= 3);
  }
  ActiveSet active_set_first = builder_first.DoneAdding();
  ActiveSet active_set_last = builder_last.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p->begin().WithActiveSet(active_set_first); it != p->end();
       ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.emplace((*it).begin(), (*it).end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  LOG(INFO) << active_set_last.DebugValues();
  for (auto it = p->begin().WithActiveSet(active_set_last); it != p->end();
       ++it) {
    LOG(INFO) << "  " << it.position();
    EXPECT_THAT(it.position(), position + 3);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkipsShredded) {
  constexpr int kPermuterSize = 3;
  auto p = TypeParam()(kPermuterSize);
  EXPECT_THAT(p->permutation_count(), 6);

  ActiveSet::Builder builder_odd(6);
  ActiveSet::Builder builder_even(6);
  for (int i = 0; i < 6; ++i) {
    builder_odd.Add(i & 1);
    builder_even.Add(!(i & 1));
  }
  ActiveSet active_set_odd = builder_odd.DoneAdding();
  ActiveSet active_set_even = builder_even.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p->begin().WithActiveSet(active_set_even); it != p->end();
       ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  for (auto it = p->begin().WithActiveSet(active_set_odd); it != p->end();
       ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ThreeElementsWithSkipsShreddedByBeginArg) {
  constexpr int kPermuterSize = 3;
  auto p = TypeParam()(kPermuterSize);
  EXPECT_THAT(p->permutation_count(), 6);

  ActiveSet::Builder builder_odd(6);
  ActiveSet::Builder builder_even(6);
  for (int i = 0; i < 6; ++i) {
    builder_odd.Add(i & 1);
    builder_even.Add(!(i & 1));
  }
  ActiveSet active_set_odd = builder_odd.DoneAdding();
  ActiveSet active_set_even = builder_even.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p->begin().WithActiveSet(active_set_even); it != p->end();
       ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  for (auto it = p->begin().WithActiveSet(active_set_odd); it != p->end();
       ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.emplace(it->begin(), it->end()).second)
        << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(0, 1, 2));
    ++position;
  }
  EXPECT_THAT(position, 3);
}

TYPED_TEST(ClassPermuterTest, ValueSkip) {
  constexpr int kPermuterSize = 9;
  auto p = TypeParam()(kPermuterSize);
  const int permutations = p->permutation_count();

  for (int value_index = 0; value_index < kPermuterSize; ++value_index) {
    int loop_count = 0;
    int last_val = -1;
    ClassPermuter::iterator::ValueSkip value_skip;
    value_skip.value_index = value_index;
    for (auto it = p->begin(); it != p->end(); it += value_skip) {
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
  constexpr int kPermuterSize = 9;
  auto p = TypeParam()(kPermuterSize);
  const int permutations = p->permutation_count();

  int loop_count = 0;
  ClassPermuter::iterator::ValueSkip value_skip;
  value_skip.value_index = Entry::kBadId;
  for (auto it = p->begin(); it != p->end(); it += value_skip) {
    ++loop_count;
  }
  EXPECT_EQ(loop_count, permutations);
}

TYPED_TEST(ClassPermuterTest, ValueSkipWithActiveSet) {
  constexpr int kPermuterSize = 4;
  auto p = TypeParam()(kPermuterSize);

  ActiveSet::Builder builder(p->permutation_count());
  for (absl::Span<const int> permutation : *p) {
    builder.Add(permutation[1] == 3);
  }
  ActiveSet only_three_in_position_one = builder.DoneAdding();
  EXPECT_EQ(only_three_in_position_one.matches(), 6);

  int loop_count = 0;
  for (auto it = p->begin().WithActiveSet(only_three_in_position_one);
       it != p->end(); ++it) {
    ++loop_count;
  }
  EXPECT_EQ(loop_count, only_three_in_position_one.matches());

  loop_count = 0;
  ClassPermuter::iterator::ValueSkip value_skip;
  value_skip.value_index = 1;
  for (auto it = p->begin().WithActiveSet(only_three_in_position_one);
       it != p->end(); it += value_skip) {
    EXPECT_EQ((*it)[1], 3);
    ++loop_count;
  }
  EXPECT_EQ(loop_count, 1);
}

TYPED_TEST(ClassPermuterTest, EmptyActiveSet) {
  constexpr int kPermuterSize = 4;
  auto p = TypeParam()(kPermuterSize);

  ActiveSet::Builder builder(p->permutation_count());
  builder.AddBlock(false, p->permutation_count());
  ActiveSet set = builder.DoneAdding();

  bool was_advanced;
  auto it = p->begin().WithActiveSet(set, &was_advanced);
  EXPECT_TRUE(was_advanced);
  EXPECT_TRUE(it == p->end())
      << it.position() << ": " << absl::StrJoin(*it, ",");
}

TYPED_TEST(ClassPermuterTest, FullActiveSet) {
  constexpr int kPermuterSize = 4;
  auto p = TypeParam()(kPermuterSize);

  ActiveSet::Builder builder(p->permutation_count());
  builder.AddBlock(true, p->permutation_count());
  ActiveSet set = builder.DoneAdding();

  int loop_count = 0;
  bool was_advanced;
  auto it = p->begin().WithActiveSet(set, &was_advanced);
  EXPECT_FALSE(was_advanced);
  for (; it != p->end(); ++it) {
    ++loop_count;
  }
  EXPECT_EQ(loop_count, p->permutation_count());
}

TYPED_TEST(ClassPermuterTest, EmptyActiveSetMidIteration) {
  constexpr int kPermuterSize = 4;
  auto p = TypeParam()(kPermuterSize);

  ActiveSet::Builder builder(p->permutation_count());
  builder.AddBlock(false, p->permutation_count());
  ActiveSet set = builder.DoneAdding();

  int loop_count = 0;
  for (auto it = p->begin(); it != p->end();) {
    ++loop_count;
    if (loop_count == 4) {
      bool was_advanced;
      it = std::move(it).WithActiveSet(set, &was_advanced);
      EXPECT_TRUE(was_advanced);
    } else {
      ++it;
    }
  }
  EXPECT_EQ(loop_count, 4);
}

TYPED_TEST(ClassPermuterTest, ActiveSetMidIteration) {
  auto p = TypeParam()(4);

  ActiveSet::Builder builder(p->permutation_count());
  builder.AddBlock(false, p->permutation_count() * 3 / 4);
  builder.AddBlock(true, p->permutation_count() / 4);
  ActiveSet set = builder.DoneAdding();

  int loop_count = 0;
  for (auto it = p->begin(); it != p->end();) {
    ++loop_count;
    if (loop_count == p->permutation_count() / 4) {
      bool was_advanced;
      it = std::move(it).WithActiveSet(set, &was_advanced);
      EXPECT_TRUE(was_advanced);
    } else {
      ++it;
    }
  }
  EXPECT_EQ(loop_count, p->permutation_count() / 2);
}

}  // namespace puzzle
