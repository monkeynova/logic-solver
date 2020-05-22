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

TEST(ActiveSet, EmptyIsTrivial) { EXPECT_TRUE(ActiveSet().is_trivial()); }

TEST(ActiveSet, SingleFalseIsNotTrivial) {
  ActiveSet single_false;
  single_false.Add(false);
  EXPECT_FALSE(single_false.is_trivial());
}

TEST(ActiveSet, SingleTrueIsNotTrivial) {
  ActiveSet single_true;
  single_true.Add(true);
  EXPECT_TRUE(single_true.is_trivial());
  single_true.DoneAdding();
  EXPECT_TRUE(single_true.is_trivial());
}

TEST(ActiveSet, ConsumeNextAllTrue) {
  ActiveSet set;
  for (int i = 0; i < 3; ++i) {
    set.Add(true);
  }
  set.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(set.ConsumeNext());
  }
}

TEST(ActiveSet, ConsumeNextAllFalse) {
  ActiveSet set;
  for (int i = 0; i < 3; ++i) {
    set.Add(false);
  }
  set.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_FALSE(set.ConsumeNext());
  }
}

TEST(ActiveSet, ConsumeNextAlternating) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 1);
  }
  set.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, DiscardFirstBlock) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 1);
  }
  set.DoneAdding();
  EXPECT_EQ(set.DiscardBlock(20), (19 & 1));
  for (int i = 20; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, DiscardBlockAlternating) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 1);
  }
  set.DoneAdding();
  for (int i = 0; i < 15; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
  EXPECT_EQ(set.DiscardBlock(20), (34 & 1));
  for (int i = 35; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(i & 1));
  }
}

TEST(ActiveSet, ConsumeNextStreaks) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 4);
  }
  set.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
}

TEST(ActiveSet, DiscardBlockStreaks) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 4);
  }
  set.DoneAdding();
  for (int i = 0; i < 10; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
  EXPECT_EQ(set.DiscardBlock(20), !!(29 & 4));
  for (int i = 30; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNext(), Eq(!!(i & 4)));
  }
}

TEST(ActiveSet, ConsumeFalseBlockFalse) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(false);
  }
  set.DoneAdding();
  EXPECT_THAT(set.ConsumeFalseBlock(), 40) << set.DebugString();
  EXPECT_THAT(set.ConsumeFalseBlock(), 0) << set.DebugString();
}

TEST(ActiveSet, ConsumeFalseBlockTrue) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(true);
  }
  set.DoneAdding();
  EXPECT_THAT(set.ConsumeFalseBlock(), 0);
}

TEST(ActiveSet, ConsumeFalseBlockStreaks) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 4);
  }
  set.DoneAdding();
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
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(!(i & 4));
  }
  set.DoneAdding();
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
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(i & 1);
  }
  set.DoneAdding();
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
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(!(i & 1));
  }
  set.DoneAdding();
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

std::set<int> Drain(ActiveSet s, int max_positions) {
  std::set<int> ret;
  for (int i = 0; i < max_positions; ++i) {
    if (s.ConsumeNext()) {
      ret.insert(i);
    }
  }
  return ret;
}

TEST(ActiveSet, SetConstruction) {
  EXPECT_THAT(Drain(ActiveSet({0}, 1), 1), ElementsAre(0));
  EXPECT_THAT(Drain(ActiveSet({}, 1), 1), ElementsAre());
  for (int i = 0; i < 4; ++i) {
    EXPECT_THAT(Drain(ActiveSet({i}, 4), 4), ElementsAre(i));
  }
  for (int i = 0; i < 5; ++i) {
    if (i == 5) continue;
    EXPECT_THAT(Drain(ActiveSet({i, 5}, 9), 9), ElementsAre(i, 5));
  }
  for (int i = 6; i < 9; ++i) {
    EXPECT_THAT(Drain(ActiveSet({i, 5}, 9), 9), ElementsAre(5, i));
  }
  EXPECT_THAT(Drain(ActiveSet({1, 3, 5, 7}, 9), 9), ElementsAre(1, 3, 5, 7));
  EXPECT_THAT(Drain(ActiveSet({0, 2, 4, 6, 8}, 9), 9),
              ElementsAre(0, 2, 4, 6, 8));
}

std::set<int> ExtractValues(ActiveSet a) {
  std::set<int> ret;
  for (int i = 0; i < a.total(); ++i) {
    if (a.ConsumeNext()) {
      ret.insert(i);
    }
  }
  return ret;
}

TEST(ActiveSet, SetConstuctionFullExact) {
  EXPECT_THAT(ExtractValues(ActiveSet({0, 1, 2, 3}, 4)),
              ElementsAre(0, 1, 2, 3));
}
TEST(ActiveSet, SetConstuctionEmptyEnd) {
  EXPECT_THAT(ExtractValues(ActiveSet({0, 1, 2, 3}, 5)),
              ElementsAre(0, 1, 2, 3));
}
TEST(ActiveSet, SetConstuctionTruncate) {
  EXPECT_THAT(ExtractValues(ActiveSet({0, 1, 2, 3}, 3)), ElementsAre(0, 1, 2));
}
TEST(ActiveSet, SetConstuctionNegative) {
  EXPECT_THAT(ExtractValues(ActiveSet({-1, 1}, 2)), ElementsAre(1));
}
TEST(ActiveSet, SetConstuctionSpards) {
  EXPECT_THAT(ExtractValues(ActiveSet({5, 10}, 100)), ElementsAre(5, 10));
}

void TestIntersection(const std::vector<int>& set_a,
                      const std::vector<int>& set_b, int max_position_a,
                      int max_position_b) {
  ActiveSet a(set_a, max_position_a);
  ActiveSet b(set_b, max_position_b);
  std::vector<int> intersection;
  std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
                        std::back_inserter(intersection));

  EXPECT_THAT(ExtractValues(a.Intersection(b)), ElementsAreArray(intersection));
  EXPECT_THAT(ExtractValues(b.Intersection(a)), ElementsAreArray(intersection));
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

TEST(ActiveSet, EnabledValues) {
  std::vector<std::vector<int>> test_cases = {
      {0, 1, 2, 3}, {}, {1, 3}, {0, 2}, {0, 1}};

  for (const auto test : test_cases) {
    EXPECT_THAT(ActiveSet(test, 4).EnabledValues(), ElementsAreArray(test));
  }
}

}  // namespace puzzle
