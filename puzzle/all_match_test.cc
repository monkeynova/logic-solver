#include "puzzle/all_match.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace puzzle {

TEST(AllMatchTest, SingleTrue) {
  EXPECT_TRUE(
      AllMatch({SolutionFilter("", [](const Solution&) { return true; }, {})},
               Solution()));
}

TEST(AllMatchTest, SingleFalse) {
  EXPECT_FALSE(
      AllMatch({SolutionFilter("", [](const Solution&) { return false; }, {})},
               Solution()));
}

TEST(AllMatchTest, PairTrue) {
  EXPECT_TRUE(
      AllMatch({SolutionFilter("", [](const Solution&) { return true; }, {}),
                SolutionFilter("", [](const Solution&) { return true; }, {})},
               Solution()));
}

TEST(AllMatchTest, PairFalse) {
  EXPECT_FALSE(
      AllMatch({SolutionFilter("", [](const Solution&) { return true; }, {}),
                SolutionFilter("", [](const Solution&) { return false; }, {})},
               Solution()));
}

TEST(AllMatchTest, ValueSkip) {
  ClassPermuter::iterator::ValueSkip value_skip;
  std::vector<Entry> entries;
  entries.emplace_back(0, std::vector<int>{}, nullptr);
  entries.emplace_back(1, std::vector<int>{}, nullptr);
  Solution has_entries(nullptr, &entries);
  ASSERT_FALSE(AllMatch({SolutionFilter(
                             "", [](const Entry&) { return true; }, {}, 0),
                         SolutionFilter(
                             "", [](const Entry&) { return false; }, {}, 1)},
                        has_entries, -1, &value_skip));
  EXPECT_EQ(value_skip.value_index, 1);
}

}  // namespace puzzle
