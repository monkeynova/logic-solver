#include "puzzle/solution_permuter_factory.h"

#include "gflags/gflags.h"
#include "puzzle/brute_solution_permuter.h"
#include "puzzle/cropped_solution_permuter.h"

DEFINE_bool(puzzle_brute_force, false, "Brute force all possible solutions");

namespace puzzle {

std::unique_ptr<SolutionPermuter> CreateSolutionPermuter(
    EntryDescriptor* entry_descriptor, const std::vector<Solution::Cropper>& on_solution_with_class, Profiler* profiler) {
  if (FLAGS_puzzle_brute_force) {
    return absl::make_unique<BruteSolutionPermuter>(entry_descriptor);
  }
  return absl::make_unique<CroppedSolutionPermuter>(
      entry_descriptor, on_solution_with_class, profiler);
}

}  // namespace puzzle
