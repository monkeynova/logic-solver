#include "puzzle/class_permuter.h"

#include <iostream>

#include "absl/strings/str_join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/active_set.h"

using ::testing::UnorderedElementsAre;

namespace puzzle {

TEST(ClassPermuter, ThreeElements) {
  IntRangeDescriptor d(3, 5);
  ClassPermuter p(&d);
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
  IntRangeDescriptor d(3, 7);
  ClassPermuter p(&d);
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
  IntRangeDescriptor d(3, 5);
  ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  ActiveSet active_set_first;
  ActiveSet active_set_last;
  for (int i = 0; i < 6; ++i) {
    active_set_first.AddSkip(i < 3);
    active_set_last.AddSkip(i >= 3);
  }
  active_set_first.DoneAdding();
  active_set_last.DoneAdding();
  
  std::set<std::vector<int>> history;
  int position = 0;
  p.set_active_set(std::move(active_set_first));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  p.set_active_set(std::move(active_set_last));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position + 3);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);
}

TEST(ClassPermuter, ThreeElementsWithSkipsShredded) {
  IntRangeDescriptor d(3, 5);
  ClassPermuter p(&d);
  EXPECT_THAT(p.permutation_count(), 6);

  ActiveSet active_set_odd;
  ActiveSet active_set_even;
  for (int i = 0; i < 6; ++i) {
    active_set_odd.AddSkip(i & 1);
    active_set_even.AddSkip(!(i & 1));
  }
  active_set_odd.DoneAdding();
  active_set_even.DoneAdding();

  std::set<std::vector<int>> history;
  int position = 0;
  p.set_active_set(std::move(active_set_even));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);

  position = 0;
  p.set_active_set(std::move(active_set_odd));
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), 2 * position + 1);
    EXPECT_TRUE(history.insert(*it).second)
      << absl::StrJoin(*it, ", ");
    EXPECT_THAT(*it, UnorderedElementsAre(3, 4, 5));
    position++;
  }
  EXPECT_THAT(position, 3);
}

}  // namespace puzzle
