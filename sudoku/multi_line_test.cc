#include <iostream>
#include <memory>

// TODO(@monkeynova): test_case_options.cc doesn't compile for me on OSX.
// There are missing implicit casts from absl::string_view to std::string.
// Even better would be that since the places it's used don't need a copy, we
// could just convert the uses to absl::string_views.
#define USE_OPTIONS 1

#include "absl/strings/ascii.h"
#include "absl/strings/str_join.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "run_test_case_result.h"
#include "sudoku/line_board.h"
#include "test_case_options.h"

// If set, flags the test suite to ignore the result.
static inline constexpr absl::string_view kIgnoreThisTestOption =
    "ignore_this_test";

void TestLineBoard(absl::string_view test_case,
                   file_based_test_driver::RunTestCaseResult* test_result) {
  // Parse and strip off the test case's options.
  std::string test_case_without_options = std::string(test_case);

  file_based_test_driver::TestCaseOptions options;
  options.RegisterBool(kIgnoreThisTestOption, false);

  const absl::Status options_status =
      options.ParseTestCaseOptions(&test_case_without_options);
  if (!options_status.ok()) {
    // For bad test cases, prefer to return an error in the output instead
    // of crashing.
    test_result->AddTestOutput(absl::StrCat("ERROR: Failed to parse options: ",
                                            options_status.ToString()));
    return;
  }

  // Ignore? Then return straight away. The test driver will copy the entire
  // test case verbatim.
  if (options.GetBool(kIgnoreThisTestOption)) {
    test_result->set_ignore_test_output(true);
    return;
  }

  absl::StripAsciiWhitespace(&test_case_without_options);

  std::unique_ptr<::puzzle::Problem> line_board =
      ::sudoku::LineBoard::Create(test_case_without_options);
  if (line_board == nullptr) {
    test_result->AddTestOutput(absl::StrCat("ERROR: Failed to parse board: ",
                                            test_case_without_options));
    return;
  }
  line_board->Setup();
  ::puzzle::Solution answer = line_board->Solve();
  if (!answer.IsValid()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not solve puzzle: ", answer.DebugString()));
    return;
  }
  test_result->AddTestOutput(::sudoku::LineBoard::ToString(answer));
}

TEST(MultiLineTest, FileBasedTest) {
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      absl::StrCat("sudoku/", "sudoku17_sample.test"), TestLineBoard));
}
