#include "puzzle/solution_permuter/solution_permuter_factory.h"

#include "absl/flags/flag.h"
#include "puzzle/solution_permuter/allowed_value_solution_permuter.h"
#include "puzzle/solution_permuter/brute_solution_permuter.h"
#include "puzzle/solution_permuter/filtered_solution_permuter.h"

ABSL_FLAG(std::string, puzzle_solution_permuter, "filtered",
          "Selects the method of solution opermutation. Allowed values are: "
          "brute, filtered, and allowonly.");

namespace puzzle {

std::unique_ptr<SolutionPermuter> CreateSolutionPermuter(
    const EntryDescriptor* entry_descriptor, Profiler* profiler) {
  if (absl::GetFlag(FLAGS_puzzle_solution_permuter) == "brute") {
    return absl::make_unique<BruteSolutionPermuter>(entry_descriptor);
  } else if (absl::GetFlag(FLAGS_puzzle_solution_permuter) == "filtered") {
    return absl::make_unique<FilteredSolutionPermuter>(entry_descriptor,
                                                       profiler);
  } else if (absl::GetFlag(FLAGS_puzzle_solution_permuter) == "allowonly") {
    return absl::make_unique<AllowedValueSolutionPermuter>(entry_descriptor);
  } else {
    LOG(FATAL) << "Bad value for flag puzzle_solution_permuter: "
               << absl::GetFlag(FLAGS_puzzle_solution_permuter);
  }
}

}  // namespace puzzle
