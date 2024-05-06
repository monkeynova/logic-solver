#include "puzzle/filtered_solution_permuter.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "puzzle/solution_filter.h"

using ::testing::Ge;

namespace puzzle {

TEST(FilteredSolutionPermuterTest, Simple) {
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor ed(absl::make_unique<IntRangeDescriptor>(3),
                     absl::make_unique<StringDescriptor>(
                         std::vector<std::string>{"foo", "bar"}),
                     std::move(class_descriptors));

  FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  ASSERT_TRUE(p.Prepare().ok());

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), solutions.size());
    EXPECT_THAT(history.insert(absl::StrCat(*it)).second, true) << *it;
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 6 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(absl::StrCat(solution)).second, false)
        << solution;
  }
}

TEST(FilteredSolutionPermuterTest, CropFirstClass) {
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor ed(absl::make_unique<IntRangeDescriptor>(3),
                     absl::make_unique<StringDescriptor>(
                         std::vector<std::string>{"foo", "bar"}),
                     std::move(class_descriptors));

  FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  ASSERT_TRUE(
      p
          .AddFilter(SolutionFilter(
              "test", [](const Solution& s) { return s.Id(1).Class(0) == 1; },
              std::vector<int>{0}))
          .ok());
  ASSERT_TRUE(p.Prepare().ok());

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(absl::StrCat(*it)).second, true) << *it;

    EXPECT_THAT(it->Id(1).Class(0), 1);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(absl::StrCat(solution)).second, false)
        << solution;
  }
}

TEST(FilteredSolutionPermuterTest, CropLastClass) {
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor ed(absl::make_unique<IntRangeDescriptor>(3),
                     absl::make_unique<StringDescriptor>(
                         std::vector<std::string>{"foo", "bar"}),
                     std::move(class_descriptors));

  FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  ASSERT_TRUE(p.AddFilter(SolutionFilter(
                              "test",
                              [](const Solution& s) {
                                LOG(INFO) << "(1,1) => " << s.Id(0).Class(1)
                                          << std::endl;
                                return s.Id(1).Class(1) == 2;
                              },
                              std::vector<int>{1}))
                  .ok());
  ASSERT_TRUE(p.Prepare().ok());

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    LOG(INFO) << "Got Next" << std::endl;
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(absl::StrCat(*it)).second, true) << *it;

    EXPECT_THAT(it->Id(1).Class(1), 2);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 6);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(absl::StrCat(solution)).second, false)
        << solution;
  }
}

TEST(FilteredSolutionPermuterTest, CropBothClasses) {
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors;
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  class_descriptors.push_back(absl::make_unique<IntRangeDescriptor>(3));
  EntryDescriptor ed(absl::make_unique<IntRangeDescriptor>(3),
                     absl::make_unique<StringDescriptor>(
                         std::vector<std::string>{"foo", "bar"}),
                     std::move(class_descriptors));

  FilteredSolutionPermuter p(&ed, /*profiler=*/nullptr);
  ASSERT_TRUE(p.AddFilter(SolutionFilter(
                              "test",
                              [](const Solution& s) {
                                LOG(INFO) << "(0,0) => " << s.Id(0).Class(0);
                                return s.Id(0).Class(0) == 1;
                              },
                              std::vector<int>{0}))
                  .ok());
  ASSERT_TRUE(p.AddFilter(SolutionFilter(
                              "test",
                              [](const Solution& s) {
                                LOG(INFO) << "(1,1) => " << s.Id(0).Class(1);
                                return s.Id(1).Class(1) == 2;
                              },
                              std::vector<int>{1}))
                  .ok());
  ASSERT_TRUE(p.Prepare().ok());

  std::unordered_set<std::string> history;
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  std::vector<Solution> solutions;
  for (auto it = p.begin(); it != p.end(); ++it) {
    LOG(INFO) << "Got Next";
    EXPECT_THAT(it.position(), Ge(solutions.size()));
    EXPECT_THAT(history.insert(absl::StrCat(*it)).second, true) << *it;

    EXPECT_THAT(it->Id(0).Class(0), 1);
    EXPECT_THAT(it->Id(1).Class(1), 2);
    solutions.emplace_back(it->Clone());
  }
  EXPECT_THAT(solutions.size(), 2 * 2);
  for (const auto& solution : solutions) {
    EXPECT_THAT(history.insert(absl::StrCat(solution)).second, false)
        << solution;
  }
}

}  // namespace puzzle
