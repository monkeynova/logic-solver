#ifndef PUZZLE_SOLUTION_PERMUTER_SOLUTION_PERMUTER_FACTORY_H
#define PUZZLE_SOLUTION_PERMUTER_SOLUTION_PERMUTER_FACTORY_H

#include <memory>

#include "puzzle/base/profiler.h"
#include "puzzle/solution_permuter/solution_permuter.h"

namespace puzzle {

std::unique_ptr<SolutionPermuter> CreateSolutionPermuter(
    const EntryDescriptor* entry_descriptor, Profiler* profiler);

}  // namespace puzzle

#endif  //  PUZZLE_SOLUTION_PERMUTER_SOLUTION_PERMUTER_FACTORY_H
