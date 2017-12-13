#include "puzzle/class_permuter.h"

#include <iostream>

#include "absl/strings/str_join.h"
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
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
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
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5, 6, 7));
    position++;
  }
  EXPECT_THAT(position, 120);
}

TEST(ClassPermuter, ThreeElementsWithSkips) {
  Puzzle::IntRangeDescriptor d(3, 5);
  Puzzle::ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin({3,3}); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  for (auto it = p.begin({0,3,3}); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position + 3);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);
}

TEST(ClassPermuter, ThreeElementsWithSkipsShredded) {
  Puzzle::IntRangeDescriptor d(3, 5);
  Puzzle::ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  std::set<std::vector<int>> history;
  int position = 0;
  for (auto it = p.begin({1,1,1,1,1,1}); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  for (auto it = p.begin({0,1,1,1,1,1,1}); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);
}
