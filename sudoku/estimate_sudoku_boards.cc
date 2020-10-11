#include <iostream>
#include <memory>

#include "absl/flags/flag.h"
#include "main_lib.h"
#include "sudoku/base.h"

ABSL_FLAG(int, solutions, 1000,
          "The number of sudoku solutions to compute to estimate the "
          "percentage of soution boards.");

extern absl::Flag<bool> FLAGS_puzzle_prune_pair_class_iterators;

class EmptySudoku : public sudoku::Base {
 public:
  absl::StatusOr<Board> GetInstanceBoard() const override { 
    return EmptyBoard();
  }
  absl::StatusOr<Board> GetSolutionBoard() const override {
   return EmptyBoard();
  }
};

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  // Pruning pair class iterators with an empty board involves basically
  // solving all sudoku problems before being able to return any. For
  // estimation purposes we disable this pessimization.
  absl::SetFlag(&FLAGS_puzzle_prune_pair_class_iterators, false);

  auto empty_sudoku = absl::WrapUnique<puzzle::Problem>(new EmptySudoku());
  CHECK(empty_sudoku->Setup().ok());

  const int num_solutions = absl::GetFlag(FLAGS_solutions);
  absl::StatusOr<std::vector<puzzle::Solution>> solutions =
   empty_sudoku->AllSolutions(num_solutions);
  CHECK(solutions.ok());
  CHECK_EQ(solutions->size(), num_solutions);

  int i = num_solutions;
  const auto& solution = solutions->at(i - 1);
    CHECK(solution.IsValid());
    std::cout << solution.permutation_position() << " of "
              << solution.permutation_count() << " => "
              << (solution.permutation_count() * i
                  / (solution.permutation_position() + 1))
              << std::endl;

  return 0;
}
