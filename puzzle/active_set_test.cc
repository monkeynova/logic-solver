#include "puzzle/active_set.h"

#include <iostream>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AnyOf;
using ::testing::Eq;

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


}  // namespace puzzle
