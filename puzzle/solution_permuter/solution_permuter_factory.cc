#include "puzzle/solution_permuter/solution_permuter_factory.h"

#include "absl/flags/flag.h"
#include "puzzle/solution_permuter/brute_solution_permuter.h"
#include "puzzle/solution_permuter/filtered_solution_permuter.h"

ABSL_FLAG(bool, puzzle_brute_force, false,
          "Brute force all possible solutions");

namespace puzzle {

std::unique_ptr<SolutionPermuter> CreateSolutionPermuter(
    const EntryDescriptor* entry_descriptor, Profiler* profiler) {
  if (absl::GetFlag(FLAGS_puzzle_brute_force)) {
    return absl::make_unique<BruteSolutionPermuter>(entry_descriptor);
  }
  return absl::make_unique<FilteredSolutionPermuter>(entry_descriptor,
                                                     profiler);
}

}  // namespace puzzle
