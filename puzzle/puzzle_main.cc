#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "main_lib.h"
#include "puzzle/problem.h"

ABSL_FLAG(bool, all, false, "Show all solutions");

std::string PositionHeader(const puzzle::Solution& s) {
  return absl::StrCat("[position=", s.permutation_position(), "/",
                      s.permutation_count(), " (", s.Completion(), ")]");
}

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  CHECK(problem != nullptr) << "No puzzle found";

  CHECK(problem->Setup().ok());

  int exit_code = 1;

  if (absl::GetFlag(FLAGS_all)) {
    LOG(INFO) << "[AllSolutions]";
    absl::StatusOr<std::vector<puzzle::Solution>> all_solutions =
        problem->AllSolutions();
    CHECK(all_solutions.ok()) << all_solutions.status();
    exit_code = all_solutions->size() > 0 ? 0 : 1;
    LOG(INFO) << "[" << all_solutions->size() << " solutions]";
    LOG(INFO) << absl::StrJoin(
        *all_solutions, "\n", [](std::string* out, const puzzle::Solution& s) {
          absl::StrAppend(out, PositionHeader(s), "\n", s.DebugString());
        });
  } else {
    absl::StatusOr<puzzle::Solution> answer = problem->Solve();
    CHECK(answer.ok()) << answer.status();
    if (answer->IsValid()) {
      LOG(INFO) << PositionHeader(*answer);
    }
    std::cout << answer->DebugString() << std::endl;
    exit_code = answer->IsValid() ? 0 : 1;
  }

  LOG(INFO) << problem->DebugStatistics();

  return exit_code;
}
