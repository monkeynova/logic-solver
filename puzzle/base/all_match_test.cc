#include "puzzle/base/all_match.h"

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
  ValueSkip value_skip;
  std::vector<Entry> entries;
  entries.emplace_back(0, std::vector<int>{});
  entries.emplace_back(1, std::vector<int>{});
  Solution has_entries(nullptr, &entries);
  ASSERT_FALSE(AllMatch({SolutionFilter(
                             "", [](const Entry&) { return true; }, {}, 0),
                         SolutionFilter(
                             "", [](const Entry&) { return false; }, {}, 1)},
                        has_entries, -1, value_skip));
  EXPECT_EQ(value_skip.value_index, 1);
}

TEST(UnmatchedEntrySkipsTest, AllTrue) {
  EXPECT_EQ(UnmatchedEntrySkips(
                {SolutionFilter("", [](const Solution&) { return true; }, {}),
                 SolutionFilter("", [](const Solution&) { return true; }, {})},
                Solution()),
            0);
}

TEST(UnmatchedEntrySkipsTest, AllFalse) {
  std::vector<Entry> entries;
  entries.emplace_back(0, std::vector<int>{});
  entries.emplace_back(1, std::vector<int>{});
  Solution has_entries(nullptr, &entries);
  EXPECT_EQ(
      UnmatchedEntrySkips({SolutionFilter(
                               "", [](const Entry&) { return false; }, {}, 0),
                           SolutionFilter(
                               "", [](const Entry&) { return false; }, {}, 1)},
                          has_entries),
      3);
}

TEST(UnmatchedEntrySkipsTest, PartialFirstFalse) {
  std::vector<Entry> entries;
  entries.emplace_back(0, std::vector<int>{});
  entries.emplace_back(1, std::vector<int>{});
  Solution has_entries(nullptr, &entries);
  EXPECT_EQ(
      UnmatchedEntrySkips({SolutionFilter(
                               "", [](const Entry&) { return false; }, {}, 0),
                           SolutionFilter(
                               "", [](const Entry&) { return true; }, {}, 1)},
                          has_entries),
      1);
}

TEST(UnmatchedEntrySkipsTest, PartialSecondFalse) {
  std::vector<Entry> entries;
  entries.emplace_back(0, std::vector<int>{});
  entries.emplace_back(1, std::vector<int>{});
  Solution has_entries(nullptr, &entries);
  EXPECT_EQ(
      UnmatchedEntrySkips({SolutionFilter(
                               "", [](const Entry&) { return true; }, {}, 0),
                           SolutionFilter(
                               "", [](const Entry&) { return false; }, {}, 1)},
                          has_entries),
      2);
}

}  // namespace puzzle
