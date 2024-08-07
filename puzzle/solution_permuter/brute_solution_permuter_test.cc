#include "puzzle/solution_permuter/brute_solution_permuter.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/base/owned_solution.h"

namespace puzzle {

TEST(BruteSolutionPermuterTest, Simple) {
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor ed(absl::make_unique<IntRangeDescriptor>(3),
                     absl::make_unique<StringDescriptor>(
                         std::vector<std::string>{"foo", "bar"}),
                     std::move(class_descriptors));

  BruteSolutionPermuter p(&ed);
  ASSERT_TRUE(p.Prepare().ok());

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<OwnedSolution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it->position().position, solutions.size());
    EXPECT_THAT(history.insert(absl::StrCat(*it)).second, true) << *it;
    solutions.push_back(OwnedSolution(*it));
  }
  EXPECT_THAT(solutions.size(), 6 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(absl::StrCat(solution)).second, false)
        << solution;
  }
}

}  // namespace puzzle
