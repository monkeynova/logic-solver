#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "main_lib.h"
#include "sudoku/sudoku.h"

ABSL_FLAG(int, solutions, 1000,
          "The number of sudoku solutions to compute to estimate the "
          "percentage of soution boards.");

extern absl::Flag<bool> FLAGS_puzzle_prune_pair_class_iterators;

class EmptySudoku : public sudoku::Sudoku {
 public:
  absl::StatusOr<Board> GetInstanceBoard() const override {
    return EmptyBoard();
  }
  absl::StatusOr<Board> GetSolutionBoard() const override {
    return EmptyBoard();
  }
};

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat("Estimates the number of total sudoku solution boards. No "
                   "arguments are allowed. Usage:\n",
                   argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument!" << std::endl
                            << absl::ProgramUsageMessage();

  // Pruning pair class iterators with an empty board involves basically
  // solving all sudoku problems before being able to return any. For
  // estimation purposes we disable this pessimization.
  absl::SetFlag(&FLAGS_puzzle_prune_pair_class_iterators, false);

  auto empty_sudoku = absl::WrapUnique<puzzle::Problem>(new EmptySudoku());
  absl::Status setup_status = empty_sudoku->Setup();
  QCHECK(setup_status.ok()) << setup_status;

  const int num_solutions = absl::GetFlag(FLAGS_solutions);
  absl::StatusOr<std::vector<puzzle::OwnedSolution>> solutions =
      empty_sudoku->AllSolutions(num_solutions);
  QCHECK(solutions.ok()) << solutions.status();
  QCHECK_EQ(solutions->size(), num_solutions);

  int i = num_solutions;
  const auto& solution = solutions->at(i - 1);
  QCHECK(solution.IsValid());
  puzzle::Position position = solution.position();
  std::cout << position.position << " of " << position.count << " => "
            << (position.count * i / (position.position + 1)) << std::endl;

  return 0;
}
