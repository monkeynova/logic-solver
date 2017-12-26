#include "puzzle/active_set.h"

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;

namespace puzzle {

TEST(ActiveSet, EmptyIsTrivial) {
  EXPECT_TRUE(ActiveSet().is_trivial());
}

TEST(ActiveSet, SingleFalseIsNotTrivial) {
  ActiveSet single_false;
  single_false.AddSkip(false);
  EXPECT_FALSE(single_false.is_trivial());
}

TEST(ActiveSet, SingleTrueIsNotTrivial) {
  ActiveSet single_true;
  single_true.AddSkip(true);
  EXPECT_TRUE(single_true.is_trivial());
  single_true.DoneAdding();
  EXPECT_TRUE(single_true.is_trivial());
}

TEST(ActiveSet, ConsumeNextSkipAllTrue) {
  ActiveSet set;
  for (int i = 0; i < 3; ++i) {
    set.AddSkip(true);
  }
  set.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(set.ConsumeNextSkip());
  } 
}

TEST(ActiveSet, ConsumeNextSkipAllFalse) {
  ActiveSet set;
  for (int i = 0; i < 3; ++i) {
    set.AddSkip(false);
  }
  set.DoneAdding();
  for (int i = 0; i < 3; ++i) {
    EXPECT_FALSE(set.ConsumeNextSkip());
  } 
}

TEST(ActiveSet, ConsumeNextSkipAlternating) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.AddSkip(i & 1);
  }
  set.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNextSkip(), Eq(i & 1));
  } 
}

TEST(ActiveSet, ConsumeNextSkipStreaks) {
  ActiveSet set;
  for (int i = 0; i < 40; ++i) {
    set.AddSkip(i & 4);
  }
  set.DoneAdding();
  for (int i = 0; i < 40; ++i) {
    EXPECT_THAT(set.ConsumeNextSkip(), Eq(!!(i & 4)));
  } 
}

}  // namespace puzzle
