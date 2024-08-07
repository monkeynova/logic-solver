#include <iostream>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "main_lib.h"
#include "puzzle/problem.h"

ABSL_FLAG(bool, all, false, "Show all solutions");

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat("Runs the given puzzle. No arguments are allowed. Usage:\n",
                   argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument!" << std::endl
                            << absl::ProgramUsageMessage();

  std::unique_ptr<puzzle::Problem> problem = puzzle::Problem::GetInstance();
  QCHECK(problem != nullptr) << "No puzzle found";

  absl::Status setup_status = problem->Setup();
  QCHECK(setup_status.ok()) << setup_status;

  int exit_code = 1;

  if (absl::GetFlag(FLAGS_all)) {
    LOG(INFO) << "[AllSolutions]";
    absl::StatusOr<std::vector<puzzle::OwnedSolution>> all_solutions =
        problem->AllSolutions();
    QCHECK(all_solutions.ok()) << all_solutions.status();
    exit_code = all_solutions->size() > 0 ? 0 : 1;
    LOG(INFO) << "[" << all_solutions->size() << " solutions]";
    LOG(INFO) << absl::StrJoin(
        *all_solutions, "\n",
        [](std::string* out, const puzzle::OwnedSolution& s) {
          absl::StrAppend(out, "[position=%v]\n%v", s.position(), s);
        });
  } else {
    absl::StatusOr<puzzle::OwnedSolution> answer = problem->Solve();
    QCHECK(answer.ok()) << answer.status();
    if (answer->IsValid()) {
      LOG(INFO) << "[position=" << answer->position() << "]";
    }
    std::cout << answer << std::endl;
    exit_code = answer->IsValid() ? 0 : 1;
  }

  LOG(INFO) << problem->DebugStatistics();

  return exit_code;
}
