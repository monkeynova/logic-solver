#include <iostream>
#include <memory>

// TODO(@monkeynova): test_case_options.cc doesn't compile for me on OSX.
// There are missing implicit casts from absl::string_view to std::string.
// Even better would be that since the places it's used don't need a copy, we
// could just convert the uses to absl::string_views.
#define USE_OPTIONS 0

#include "absl/strings/ascii.h"
#include "absl/strings/str_join.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "run_test_case_result.h"
#include "sudoku/line_board.h"
#if USE_OPTIONS
#include "test_case_options.h"
#endif

void TestLineBoard(absl::string_view test_case,
                   file_based_test_driver::RunTestCaseResult* test_result) {
  // Parse and strip off the test case's options.
  std::string test_case_without_options = std::string(test_case);

#if USE_OPTIONS
  const absl::Status options_status =
      options_.ParseTestCaseOptions(&test_case_without_options);
  if (!options_status.ok()) {
    // For bad test cases, prefer to return an error in the output instead
    // of crashing.
    test_result->AddTestOutput(absl::StrCat("ERROR: Failed to parse options: ",
                                            options_status.ToString()));
    return;
  }

  // Ignore? Then return straight away. The test driver will copy the entire
  // test case verbatim.
  if (options_.GetBool(kIgnoreThisTestOption)) {
    test_result->set_ignore_test_output(true);
    return;
  }
#endif

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
  // const std::string filespec =
  // file_based_test_driver_base::JoinPath(TestDir(), "multi_line.test");
  const std::string filespec =
      absl::StrCat(getenv("TEST_DATADIR"), "sudoku/multi_line.test");
  EXPECT_TRUE(
      file_based_test_driver::RunTestCasesFromFiles(filespec, TestLineBoard));
}
