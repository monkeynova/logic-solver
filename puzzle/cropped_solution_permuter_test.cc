#include "puzzle/cropped_solution_permuter.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Ge;

TEST(CroppedSolutionPermuterTest, Simple) {
  Puzzle::EntryDescriptor ed;
  Puzzle::IntRangeDescriptor id(3, 5);
  Puzzle::IntRangeDescriptor cd1(6, 8);
  Puzzle::IntRangeDescriptor cd2(11, 13);
  
  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  Puzzle::CroppedSolutionPermuter p(&ed, /*ignored=*/{});
  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), solutions.size());
    EXPECT_THAT(history.insert(it->ToStr()).second, true)
        << it->ToStr();
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 6 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.ToStr()).second, false)
        << solution.ToStr();
  }
}

TEST(CroppedSolutionPermuterTest, CropFirstClass) {
  Puzzle::EntryDescriptor ed;
  Puzzle::IntRangeDescriptor id(0, 2);
  Puzzle::IntRangeDescriptor cd1(6, 8);
  Puzzle::IntRangeDescriptor cd2(11, 13);
  
  ed.SetIds(&id);
  ed.SetClass(0, "foo", &cd1);
  ed.SetClass(1, "bar", &cd2);

  std::vector<Puzzle::Solution::Cropper> croppers;
  croppers.emplace_back("test", 
                        [](const Puzzle::Solution& s) {
                            return s.Id(1).Class(0) == 7;
                        },
                        std::vector<int>{0});

  Puzzle::CroppedSolutionPermuter p(&ed, croppers);
  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Puzzle::Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(it->ToStr()).second, true)
        << it->ToStr();
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(solution.ToStr()).second, false)
        << solution.ToStr();
  }
}