#include "puzzle/class_permuter.h"

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(ClassPermuter, ThreeElements) {
  Puzzle::IntRangeDescriptor d(3, 5);
  Puzzle::ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.insert(*it).second);
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 6);
}

TEST(ClassPermuter, FiveElements) {
  Puzzle::IntRangeDescriptor d(3, 7);
  Puzzle::ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 120);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.insert(*it).second);
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5, 6, 7));
    position++;
  }
  EXPECT_THAT(position, 120);
}
