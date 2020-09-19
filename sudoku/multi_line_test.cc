#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_join.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/internal/gtest-port.h"
#include "gtest/gtest.h"
#include "run_test_case_result.h"
#include "sudoku/line_board.h"
#include "test_case_options.h"

// If set, flags the test suite to ignore the result.
static inline constexpr absl::string_view kIgnoreThisTestOption =
    "ignore_this_test";

ABSL_FLAG(std::string, test_filename, "",
	  "The name of the file containing sudoku problems and solutions to test.");

// TODO(@monkeynova): This is horrific in that we're poking through the
// internal abstraction to get at the original flags. Until either ABSL, googletest
// and FBTD play nice together or I can use my own main, I don't see a different
// way to initialize absl flags.
void InitializeAbslFlagsFromGtest() {
  std::vector<std::string> string_argvs = testing::internal::GetArgvs();
  std::vector<const char*> raw_argvs(string_argvs.size());
  for (int i = 0; i < string_argvs.size(); ++i) {
    raw_argvs[i] = string_argvs[i].c_str();
  }
  int argc = raw_argvs.size();
  absl::ParseCommandLine(argc, const_cast<char**>(raw_argvs.data()));
}


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
  InitializeAbslFlagsFromGtest();
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      absl::StrCat("sudoku/", absl::GetFlag(FLAGS_test_filename)), TestLineBoard));
}
