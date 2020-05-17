#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "puzzle/main_lib.h"
#include "puzzle/problem.h"

ABSL_FLAG(bool, all, false, "Show all solutions");

std::string PositionHeader(const puzzle::Solution& s) {
  return absl::StrCat("[position=", s.permutation_position(), "/",
                      s.permutation_count(), " (", s.Completion(), ")]");
}

int main(int argc, char** argv) {
  std::vector<char*> args = ::puzzle::InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  CHECK(problem != nullptr) << "No puzzle found";

  problem->Setup();

  int exit_code = 1;

  if (absl::GetFlag(FLAGS_all)) {
    LOG(INFO) << "[AllSolutions]";
    std::vector<puzzle::Solution> all_solutions = problem->AllSolutions();
    exit_code = all_solutions.size() > 0 ? 0 : 1;
    LOG(INFO) << "[" << all_solutions.size() << " solutions]";
    LOG(INFO) << absl::StrJoin(
        all_solutions, "\n", [](std::string* out, const puzzle::Solution& s) {
          absl::StrAppend(out, PositionHeader(s), "\n", s.DebugString());
        });
  } else {
    puzzle::Solution answer = problem->Solve();
    if (answer.IsValid()) {
      LOG(INFO) << PositionHeader(answer);
    }
    LOG(INFO) << answer.DebugString();
    exit_code = answer.IsValid() ? 0 : 1;
  }

  LOG(INFO) << problem->DebugStatistics();

  return exit_code;
}
