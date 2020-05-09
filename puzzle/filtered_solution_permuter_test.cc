#include "puzzle/filtered_solution_permuter.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Ge;

TEST(FilteredSolutionPermuterTest, Simple) {
  puzzle::EntryDescriptor ed;
  puzzle::IntRangeDescriptor id(3, 5);
  puzzle::IntRangeDescriptor cd1(6, 8);
  puzzle::IntRangeDescriptor cd2(11, 13);

  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  puzzle::FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  p.Prepare();

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), solutions.size());
    EXPECT_THAT(history.insert(it->DebugString()).second, true)
        << it->DebugString();
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 6 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.DebugString()).second, false)
        << solution.DebugString();
  }
}

TEST(FilteredSolutionPermuterTest, CropFirstClass) {
  puzzle::EntryDescriptor ed;
  puzzle::IntRangeDescriptor id(0, 2);
  puzzle::IntRangeDescriptor cd1(6, 8);
  puzzle::IntRangeDescriptor cd2(11, 13);

  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  puzzle::FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  p.AddPredicate(
      "test", [](const puzzle::Solution& s) { return s.Id(1).Class(0) == 7; },
      std::vector<int>{0});
  p.Prepare();

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(it->DebugString()).second, true)
        << it->DebugString();

    EXPECT_THAT(it->Id(1).Class(0), 7);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.DebugString()).second, false)
        << solution.DebugString();
  }
}

TEST(FilteredSolutionPermuterTest, CropLastClass) {
  puzzle::EntryDescriptor ed;
  puzzle::IntRangeDescriptor id(0, 2);
  puzzle::IntRangeDescriptor cd1(6, 8);
  puzzle::IntRangeDescriptor cd2(11, 13);

  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  puzzle::FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  p.AddPredicate(
      "test",
      [](const puzzle::Solution& s) {
        LOG(INFO) << "(1,1) => " << s.Id(0).Class(1) << std::endl;
        return s.Id(1).Class(1) == 12;
      },
      std::vector<int>{1});
  p.Prepare();

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    LOG(INFO) << "Got Next" << std::endl;
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(it->DebugString()).second, true)
        << it->DebugString();

    EXPECT_THAT(it->Id(1).Class(1), 12);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.DebugString()).second, false)
        << solution.DebugString();
  }
}

TEST(FilteredSolutionPermuterTest, CropBothClasses) {
  puzzle::EntryDescriptor ed;
  puzzle::IntRangeDescriptor id(0, 2);
  puzzle::IntRangeDescriptor cd1(6, 8);
  puzzle::IntRangeDescriptor cd2(11, 13);

  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  puzzle::FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  p.AddPredicate(
      "test",
      [](const puzzle::Solution& s) {
        LOG(INFO) << "(0,0) => " << s.Id(0).Class(0);
        return s.Id(0).Class(0) == 7;
      },
      std::vector<int>{0});
  p.AddPredicate(
      "test",
      [](const puzzle::Solution& s) {
        LOG(INFO) << "(1,1) => " << s.Id(0).Class(1);
        return s.Id(1).Class(1) == 12;
      },
      std::vector<int>{1});
  p.Prepare();

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    LOG(INFO) << "Got Next";
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(it->DebugString()).second, true)
        << it->DebugString();

    EXPECT_THAT(it->Id(0).Class(0), 7);
    EXPECT_THAT(it->Id(1).Class(1), 12);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 2);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.DebugString()).second, false)
        << solution.DebugString();
  }
}
