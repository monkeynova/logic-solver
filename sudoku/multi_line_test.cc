#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/reflection.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include "run_test_case_result.h"
#include "sudoku/line_board.h"
#include "test_case_options.h"

// If set, flags the test suite to ignore the result.
static inline constexpr absl::string_view kIgnoreThisTestOption =
    "ignore_this_test";
static inline constexpr absl::string_view kFlagsOption = "flags";

ABSL_FLAG(
    std::string, test_filename, "",
    "The name of the file containing sudoku problems and solutions to test.");

ABSL_FLAG(bool, puzzle_test_unique, true,
          "If true (default), tests validate that the solution found is "
          "unique.");

// TODO(@monkeynova): This is horrific in that we're poking through the
// internal abstraction to get at the original flags. Until either ABSL,
// googletest and FBTD play nice together or I can use my own main, I don't see
// a different way to initialize absl flags.
void InitializeAbslFlagsFromGtest() {
  std::vector<std::string> string_argvs = testing::internal::GetArgvs();
  std::vector<const char*> raw_argvs(string_argvs.size());
  for (int i = 0; i < string_argvs.size(); ++i) {
    raw_argvs[i] = string_argvs[i].c_str();
  }
  int argc = raw_argvs.size();
  absl::ParseCommandLine(argc, const_cast<char**>(raw_argvs.data()));
}

void InitializeAbslFlagsFromOptions(std::string option_str) {
  for (const auto& flag_setting : absl::StrSplit(option_str, ",")) {
    std::vector<std::string> name_and_value =
        absl::StrSplit(flag_setting, absl::MaxSplits('=', 1));
    CHECK_EQ(name_and_value.size(), 2)
        << "Could not split flag and value in \"" << flag_setting << "\"";
    absl::CommandLineFlag* absl_flag =
        absl::FindCommandLineFlag(name_and_value[0]);
    CHECK(absl_flag != nullptr)
        << "Cannot find flag \"" << name_and_value[0] << "\"";
    std::string parse_error;
    CHECK(absl_flag->ParseFrom(name_and_value[1], &parse_error))
        << "Cannot parse flag \"" << name_and_value[0] << "\" from value \""
        << name_and_value[1] << "\": " << parse_error;
  }
}

void TestLineBoard(absl::string_view test_case,
                   file_based_test_driver::RunTestCaseResult* test_result) {
  // Parse and strip off the test case's options.
  std::string test_case_without_options = std::string(test_case);

  file_based_test_driver::TestCaseOptions options;
  options.RegisterBool(kIgnoreThisTestOption, false);
  options.RegisterString(kFlagsOption, "");

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

  // Ensure that if we change flags we restore them on leaving this function.
  absl::FlagSaver save_flags;

  if (options.GetString(kFlagsOption) != "") {
    InitializeAbslFlagsFromOptions(options.GetString(kFlagsOption));
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
  if (absl::GetFlag(FLAGS_puzzle_test_unique)) {
    std::unique_ptr<::puzzle::Problem> line_board =
        ::sudoku::LineBoard::Create(test_case_without_options);
    line_board->Setup();

    std::vector<puzzle::Solution> solutions =
        line_board->AllSolutions(/*limit=*/2);
    if (solutions.size() != 1) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Puzzle solution not unique"));
    }
  }
  test_result->AddTestOutput(::sudoku::LineBoard::ToString(answer));
}

TEST(MultiLineTest, FileBasedTest) {
  InitializeAbslFlagsFromGtest();
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      absl::StrCat("sudoku/", absl::GetFlag(FLAGS_test_filename)),
      TestLineBoard));
}
