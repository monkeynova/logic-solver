#include "puzzle/active_set.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AnyOf;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::Eq;

namespace puzzle {

TEST(ActiveSet, EmptyIsTrivial) {
  EXPECT_TRUE(ActiveSet::trivial().is_trivial());
}

TEST(ActiveSet, SingleFalseIsNotTrivial) {
  ActiveSetBuilder single_false;
  single_false.Add(false);
  EXPECT_FALSE(single_false.DoneAdding().is_trivial());
}

TEST(ActiveSet, SingleTrueIsNotTrivial) {
  ActiveSetBuilder single_true;
  single_true.Add(true);
  EXPECT_TRUE(single_true.DoneAdding().is_trivial());
}

TEST(ActiveSet, ConsumeNextAllTrue) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 3; ++i) {
    builder.Add(true);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(set.ConsumeNext());
  }
}

TEST(ActiveSet, ConsumeNextAllFalse) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 3; ++i) {
    builder.Add(false);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_FALSE(set.ConsumeNext());
  }
}

TEST(ActiveSet, ConsumeNextAlternating) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, DiscardFirstBlock) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  ActiveSet set = builder.DoneAdding();
  EXPECT_EQ(set.offset(), 0);
  EXPECT_EQ(set.DiscardBlock(20), (19 & 1));
  EXPECT_EQ(set.offset(), 20);
  for (int i = 20; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, DiscardBlockAlternating) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 15; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
  EXPECT_EQ(set.offset(), 15);
  EXPECT_EQ(set.DiscardBlock(20), (34 & 1));
  EXPECT_EQ(set.offset(), 35);
  for (int i = 35; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, ConsumeNextStreaks) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
}

TEST(ActiveSet, DiscardBlockStreaks) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 10; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
  EXPECT_EQ(set.DiscardBlock(20), !!(29 & 4));
  for (int i = 30; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
}

TEST(ActiveSet, ConsumeFalseBlockFalse) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(false);
  }
  ActiveSet set = builder.DoneAdding();
  EXPECT_THAT(set.ConsumeFalseBlock(), 40) << set.DebugString();
  EXPECT_EQ(set.offset(), 40);
  EXPECT_THAT(set.ConsumeFalseBlock(), 0) << set.DebugString();
  EXPECT_EQ(set.offset(), 40);
}

TEST(ActiveSet, ConsumeFalseBlockTrue) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(true);
  }
  ActiveSet set = builder.DoneAdding();
  EXPECT_THAT(set.ConsumeFalseBlock(), 0);
}

TEST(ActiveSet, ConsumeFalseBlockStreaks) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 4);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    const int delta = set.ConsumeFalseBlock();
    ASSERT_THAT(delta, AnyOf(0, 4));
    i += delta;
    if (i >= 40) break;
    EXPECT_THAT(!!(i & 4), true);
    EXPECT_THAT(set.ConsumeNext(), true) << set.DebugString();
  }
}

TEST(ActiveSet, ConsumeFalseBlockStreaksTrueFirst) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(!(i & 4));
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    const int delta = set.ConsumeFalseBlock();
    ASSERT_THAT(delta, AnyOf(0, 4));
    i += delta;
    if (i >= 40) break;
    EXPECT_THAT(!(i & 4), true);
    EXPECT_THAT(set.ConsumeNext(), true) << set.DebugString();
  }
}

TEST(ActiveSet, ConsumeFalseBlockAlternating) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(i & 1);
  }
  ActiveSet set = builder.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    const int delta = set.ConsumeFalseBlock();
    EXPECT_THAT(delta, AnyOf(0, 1));
    i += delta;
    if (i >= 40) break;
    EXPECT_THAT(!!(i & 1), true);
    EXPECT_THAT(set.ConsumeNext(), true);
  }
}

TEST(ActiveSet, ConsumeFalseBlockAlternatingTrueFirst) {
  ActiveSetBuilder builder;
  for (int i = 0; i < 40; ++i) {
    builder.Add(!(i & 1));
  }
  ActiveSet set = builder.DoneAdding();
  LOG(INFO) << set.DebugString();
  for (int i = 0; i < 40; ++i) {
    const int delta = set.ConsumeFalseBlock();
    EXPECT_THAT(delta, AnyOf(0, 1));
    i += delta;
    if (i >= 40) break;
    EXPECT_THAT(!(i & 1), true);
    EXPECT_THAT(set.ConsumeNext(), true) << i << ": " << set.DebugString();
  }
}

TEST(ActiveSet, EnabledValues) {
  std::vector<std::vector<int>> test_cases = {
      {0, 1, 2, 3}, {}, {1, 3}, {0, 2}, {0, 1}};

  for (const auto test : test_cases) {
    EXPECT_THAT(ActiveSetBuilder::FromPositions(test, 4).EnabledValues(),
                ElementsAreArray(test));
  }
}

TEST(ActiveSet, EnabledValuesMultipleCalls) {
  ActiveSet set = ActiveSetBuilder::FromPositions({0, 1, 2, 3}, 4);
  EXPECT_THAT(set.EnabledValues(), ElementsAre(0, 1, 2, 3));
  EXPECT_THAT(set.EnabledValues(), ElementsAre(0, 1, 2, 3));
}

TEST(ActiveSet, EnabledValuesAfterConsume) {
  ActiveSet set = ActiveSetBuilder::FromPositions({0, 1, 2, 3}, 4);
  EXPECT_THAT(set.EnabledValues(), ElementsAre(0, 1, 2, 3));
  LOG(INFO) << set.offset();
  set.ConsumeNext();
  LOG(INFO) << set.offset();
  EXPECT_THAT(set.EnabledValues(), ElementsAre(1, 2, 3));
}

TEST(ActiveSet, SetConstruction) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({0}, 1).EnabledValues(),
              ElementsAre(0));
  EXPECT_THAT(ActiveSetBuilder::FromPositions({}, 1).EnabledValues(),
              ElementsAre());
  for (int i = 0; i < 4; ++i) {
    EXPECT_THAT(ActiveSetBuilder::FromPositions({i}, 4).EnabledValues(),
                ElementsAre(i));
  }
  for (int i = 0; i < 5; ++i) {
    if (i == 5) continue;
    EXPECT_THAT(ActiveSetBuilder::FromPositions({i, 5}, 9).EnabledValues(),
                ElementsAre(i, 5));
  }
  for (int i = 6; i < 9; ++i) {
    EXPECT_THAT(ActiveSetBuilder::FromPositions({i, 5}, 9).EnabledValues(),
                ElementsAre(5, i));
  }
  EXPECT_THAT(ActiveSetBuilder::FromPositions({1, 3, 5, 7}, 9).EnabledValues(),
              ElementsAre(1, 3, 5, 7));
  EXPECT_THAT(
      ActiveSetBuilder::FromPositions({0, 2, 4, 6, 8}, 9).EnabledValues(),
      ElementsAre(0, 2, 4, 6, 8));
}

TEST(ActiveSet, SetConstuctionFullExact) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({0, 1, 2, 3}, 4).EnabledValues(),
              ElementsAre(0, 1, 2, 3));
}
TEST(ActiveSet, SetConstuctionEmptyEnd) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({0, 1, 2, 3}, 5).EnabledValues(),
              ElementsAre(0, 1, 2, 3));
}
TEST(ActiveSet, SetConstuctionTruncate) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({0, 1, 2, 3}, 3).EnabledValues(),
              ElementsAre(0, 1, 2));
}
TEST(ActiveSet, SetConstuctionNegative) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({-1, 1}, 2).EnabledValues(),
              ElementsAre(1));
}
TEST(ActiveSet, SetConstuctionSpards) {
  EXPECT_THAT(ActiveSetBuilder::FromPositions({5, 10}, 100).EnabledValues(),
              ElementsAre(5, 10));
}

void TestIntersection(std::vector<int> set_a, std::vector<int> set_b,
                      int max_position_a, int max_position_b) {
  ActiveSet a = ActiveSetBuilder::FromPositions(set_a, max_position_a);
  ActiveSet b = ActiveSetBuilder::FromPositions(set_b, max_position_b);
  std::vector<int> full_intersection;
  std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
                        std::back_inserter(full_intersection));
  absl::Span<int> intersection = absl::MakeSpan(full_intersection);

  for (int i = 0; i < std::min(max_position_a, max_position_b); ++i) {
    EXPECT_THAT(a.Intersection(b).EnabledValues(),
                ElementsAreArray(intersection))
        << "Offset: " << i << "; " << a.DebugString() << "; "
        << b.DebugString();
    EXPECT_THAT(b.Intersection(a).EnabledValues(),
                ElementsAreArray(intersection))
        << "Offset: " << i << "; " << a.DebugString() << "; "
        << b.DebugString();
    a.ConsumeNext();
    b.ConsumeNext();
    while (!intersection.empty() && intersection[0] <= i) {
      intersection = intersection.subspan(1);
    }
  }
}

TEST(ActiveSet, IntersectionFull) { TestIntersection({0, 1}, {0, 1}, 2, 2); }
TEST(ActiveSet, IntersectionEmpty) { TestIntersection({0}, {1}, 2, 2); }
TEST(ActiveSet, IntersectionSubset) {
  TestIntersection({0, 1, 2, 3}, {2, 3}, 4, 4);
}
TEST(ActiveSet, IntersectionSparseRangePartialMatch) {
  TestIntersection({2, 3, 5, 6, 8, 9}, {3, 5, 8}, 10, 10);
}
TEST(ActiveSet, IntersectionSparseRangeEmpty) {
  TestIntersection({2, 3, 5, 6, 8, 9}, {4, 7}, 10, 10);
}

TEST(ActiveSet, Empirical) {
  TestIntersection(
      {0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23},
      {3, 7, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23}, 24, 24);
}

}  // namespace puzzle
