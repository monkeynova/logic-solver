#include "puzzle/active_set.h"

#include <iostream>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set_bit_vector.h"
#include "puzzle/active_set_run_length.h"
#include "puzzle/active_set_run_position.h"
#include "puzzle/vlog.h"

using ::testing::AnyOf;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::Eq;

namespace puzzle {

template <typename T>
class ActiveSetTest : public ::testing::Test {};

using ActiveSetTypes = ::testing::Types<ActiveSetRunLength, ActiveSetBitVector,
                                        ActiveSetRunPosition>;
TYPED_TEST_SUITE(ActiveSetTest, ActiveSetTypes);

TYPED_TEST(ActiveSetTest, EmptyIsTrivial) {
  EXPECT_TRUE(TypeParam::trivial().is_trivial());
}

TYPED_TEST(ActiveSetTest, SingleFalseIsNotTrivial) {
  typename TypeParam::Builder single_false(1);
  single_false.Add(false);
  EXPECT_FALSE(single_false.DoneAdding().is_trivial());
}

TYPED_TEST(ActiveSetTest, SingleTrueIsNotTrivial) {
  typename TypeParam::Builder single_true(1);
  single_true.Add(true);
  EXPECT_TRUE(single_true.DoneAdding().is_trivial());
}

TYPED_TEST(ActiveSetTest, ConsumeNextAllTrue) {
  typename TypeParam::Builder builder(3);
  for (int i = 0; i < 3; ++i) {
    builder.Add(true);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 3; ++i, it.Advance(1)) {
    EXPECT_TRUE(it.value()) << i;
  }
}

TYPED_TEST(ActiveSetTest, ConsumeNextAllFalse) {
  typename TypeParam::Builder builder(3);
  for (int i = 0; i < 3; ++i) {
    builder.Add(false);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 3; ++i, it.Advance(1)) {
    EXPECT_FALSE(it.value()) << i;
  }
}

TYPED_TEST(ActiveSetTest, ConsumeNextAlternating) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 40; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(i & 1));
  }
}

TYPED_TEST(ActiveSetTest, DiscardFirstBlock) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  EXPECT_EQ(it.offset(), 0);
  it.Advance(20);
  EXPECT_EQ(it.offset(), 20);
  for (int i = 20; i < 40; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(i & 1));
  }
}

TYPED_TEST(ActiveSetTest, DiscardBlockAlternating) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 15; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(i & 1)) << i;
  }
  EXPECT_EQ(it.offset(), 15);
  it.Advance(20);
  EXPECT_EQ(it.offset(), 35);
  for (int i = 35; i < 40; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(i & 1));
  }
}

TYPED_TEST(ActiveSetTest, ConsumeNextStreaks) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 40; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(!!(i & 4)));
  }
}

TYPED_TEST(ActiveSetTest, DiscardBlockStreaks) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  for (int i = 0; i < 10; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(!!(i & 4)));
  }
  it.Advance(20);
  EXPECT_EQ(it.value(), !!(29 & 4));
  for (int i = 30; i < 40; ++i, it.Advance(1)) {
    EXPECT_THAT(it.value(), Eq(!!(i & 4)));
  }
}

TYPED_TEST(ActiveSetTest, RunSizeBlockFalse) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(false);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  EXPECT_THAT(it.RunSize(), 40) << set.DebugString();
  EXPECT_EQ(it.value(), false);
}

TYPED_TEST(ActiveSetTest, RunSizeBlockTrue) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(true);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  EXPECT_THAT(it.RunSize(), 40) << set.DebugString();
  EXPECT_EQ(it.value(), true);
}

TYPED_TEST(ActiveSetTest, RunSizeBlockStreaks) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  while (it.more()) {
    EXPECT_EQ(it.RunSize(), 4) << it.offset() << "; " << set.DebugString();
    EXPECT_EQ(it.value(), !!(it.offset() & 4));
    it.Advance(it.RunSize());
  }
  EXPECT_EQ(it.offset(), 40);
}

TYPED_TEST(ActiveSetTest, RunSizeBlockStreaksTrueFirst) {
  typename TypeParam::Builder builder(40);
  for (int i = 0; i < 40; ++i) {
    builder.Add(!(i & 4));
  }
  TypeParam set = builder.DoneAdding();
  typename TypeParam::Iterator it = set.GetIterator();
  while (it.more()) {
    EXPECT_EQ(it.RunSize(), 4) << it.offset();
    EXPECT_EQ(it.value(), !(it.offset() & 4)) << it.offset();
    it.Advance(it.RunSize());
  }
  EXPECT_EQ(it.offset(), 40);
}

TYPED_TEST(ActiveSetTest, EnabledValues) {
  std::vector<std::vector<int>> test_cases = {
      {0, 1, 2, 3}, {}, {1, 3}, {0, 2}, {0, 1}};

  for (const auto& test : test_cases) {
    EXPECT_THAT(TypeParam::Builder::FromPositions(test, 4).EnabledValues(),
                ElementsAreArray(test))
        << "{" << absl::StrJoin(test, ",") << "}";
  }
}

TYPED_TEST(ActiveSetTest, EnabledValuesMultipleCalls) {
  TypeParam set = TypeParam::Builder::FromPositions({0, 1, 2, 3}, 4);
  EXPECT_THAT(set.EnabledValues(), ElementsAre(0, 1, 2, 3));
  EXPECT_THAT(set.EnabledValues(), ElementsAre(0, 1, 2, 3));
}

TYPED_TEST(ActiveSetTest, SetConstruction) {
  EXPECT_THAT(TypeParam::Builder::FromPositions({0}, 1).EnabledValues(),
              ElementsAre(0));
  EXPECT_THAT(TypeParam::Builder::FromPositions({}, 1).EnabledValues(),
              ElementsAre());
  for (int i = 0; i < 4; ++i) {
    EXPECT_THAT(TypeParam::Builder::FromPositions({i}, 4).EnabledValues(),
                ElementsAre(i));
  }
  for (int i = 0; i < 5; ++i) {
    if (i == 5) continue;
    EXPECT_THAT(TypeParam::Builder::FromPositions({i, 5}, 9).EnabledValues(),
                ElementsAre(i, 5));
  }
  for (int i = 6; i < 9; ++i) {
    EXPECT_THAT(TypeParam::Builder::FromPositions({i, 5}, 9).EnabledValues(),
                ElementsAre(5, i));
  }
  EXPECT_THAT(
      TypeParam::Builder::FromPositions({1, 3, 5, 7}, 9).EnabledValues(),
      ElementsAre(1, 3, 5, 7));
  EXPECT_THAT(
      TypeParam::Builder::FromPositions({0, 2, 4, 6, 8}, 9).EnabledValues(),
      ElementsAre(0, 2, 4, 6, 8));
}

TYPED_TEST(ActiveSetTest, SetConstuctionFullExact) {
  EXPECT_THAT(
      TypeParam::Builder::FromPositions({0, 1, 2, 3}, 4).EnabledValues(),
      ElementsAre(0, 1, 2, 3));
}
TYPED_TEST(ActiveSetTest, SetConstuctionEmptyEnd) {
  EXPECT_THAT(
      TypeParam::Builder::FromPositions({0, 1, 2, 3}, 5).EnabledValues(),
      ElementsAre(0, 1, 2, 3));
}
TYPED_TEST(ActiveSetTest, SetConstuctionTruncate) {
  EXPECT_THAT(
      TypeParam::Builder::FromPositions({0, 1, 2, 3}, 3).EnabledValues(),
      ElementsAre(0, 1, 2));
}
TYPED_TEST(ActiveSetTest, SetConstuctionNegative) {
  EXPECT_THAT(TypeParam::Builder::FromPositions({-1, 1}, 2).EnabledValues(),
              ElementsAre(1));
}
TYPED_TEST(ActiveSetTest, SetConstuctionSpards) {
  EXPECT_THAT(TypeParam::Builder::FromPositions({5, 10}, 100).EnabledValues(),
              ElementsAre(5, 10));
}

template <typename TypeParam>
void TestIntersection(std::vector<int> set_a, std::vector<int> set_b,
                      int max_position_a, int max_position_b) {
  TypeParam a = TypeParam::Builder::FromPositions(set_a, max_position_a);
  TypeParam b = TypeParam::Builder::FromPositions(set_b, max_position_b);
  std::vector<int> full_intersection;
  std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
                        std::back_inserter(full_intersection));
  absl::Span<int> intersection = absl::MakeSpan(full_intersection);

  EXPECT_THAT(a.Intersection(b).EnabledValues(), ElementsAreArray(intersection))
      << a.DebugString() << " /\\ " << b.DebugString() << " = "
      << a.Intersection(b).DebugString();
  EXPECT_EQ(
      a.Intersection(b).Selectivity(),
      1.0 * intersection.size() / std::max(max_position_a, max_position_b));
  EXPECT_THAT(b.Intersection(a).EnabledValues(), ElementsAreArray(intersection))
      << b.DebugString() << " /\\ " << a.DebugString() << " = "
      << b.Intersection(a).DebugString();
  EXPECT_EQ(
      b.Intersection(a).Selectivity(),
      1.0 * intersection.size() / std::max(max_position_a, max_position_b));
}

TYPED_TEST(ActiveSetTest, IntersectionFull) {
  TestIntersection<TypeParam>({0, 1}, {0, 1}, 2, 2);
}
TYPED_TEST(ActiveSetTest, IntersectionEmpty) {
  TestIntersection<TypeParam>({0}, {1}, 2, 2);
}
TYPED_TEST(ActiveSetTest, IntersectionSubset) {
  TestIntersection<TypeParam>({0, 1, 2, 3}, {2, 3}, 4, 4);
}
TYPED_TEST(ActiveSetTest, IntersectionSparseRangePartialMatch) {
  TestIntersection<TypeParam>({2, 3, 5, 6, 8, 9}, {3, 5, 8}, 10, 10);
}
TYPED_TEST(ActiveSetTest, IntersectionSparseRangeEmpty) {
  TestIntersection<TypeParam>({2, 3, 5, 6, 8, 9}, {4, 7}, 10, 10);
}

TYPED_TEST(ActiveSetTest, Empirical) {
  TestIntersection<TypeParam>(
      {0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23},
      {3, 7, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23}, 24, 24);
}

}  // namespace puzzle
