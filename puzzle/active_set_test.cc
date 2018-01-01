#include "puzzle/active_set.h"

#include <iostream>
#include <set>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AnyOf;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::UnorderedElementsAre;

namespace puzzle {

TEST(ActiveSet, EmptyIsTrivial) {
  EXPECT_TRUE(ActiveSet().is_trivial());
}

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

TEST(ActiveSet, ConsumeFalseBlockFalse) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.Add(false);
  }
  set.DoneAdding();
  EXPECT_THAT(set.ConsumeFalseBlock(), 40)
    << set.DebugString();
  EXPECT_THAT(set.ConsumeFalseBlock(), 0)
    << set.DebugString();
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
    EXPECT_THAT(set.ConsumeNext(), true)
      << set.DebugString();
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
    EXPECT_THAT(set.ConsumeNext(), true)
      << set.DebugString();
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
    EXPECT_THAT(set.ConsumeNext(), true)
      << i << ": " << set.DebugString();
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
  EXPECT_THAT(Drain(ActiveSet({0}, 1), 1),
              ElementsAre(0));
  EXPECT_THAT(Drain(ActiveSet({}, 1), 1),
              ElementsAre());
  for (int i = 0; i < 4; ++i) {
    EXPECT_THAT(Drain(ActiveSet({i}, 4), 4),
                ElementsAre(i));
  }
  for (int i = 0; i < 9; ++i) {
    if (i == 5) continue;
    EXPECT_THAT(Drain(ActiveSet({i, 5}, 9), 9),
                UnorderedElementsAre(i, 5));
  }
  EXPECT_THAT(Drain(ActiveSet({1,3,5,7}, 9), 9),
              ElementsAre(1,3,5,7));
  EXPECT_THAT(Drain(ActiveSet({0,2,4,6,8}, 9), 9),
              ElementsAre(0,2,4,6,8));
}


}  // namespace puzzle
