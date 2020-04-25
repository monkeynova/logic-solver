#ifndef __PUZZLE_SOLUTION_PERMUTER_FACTORY_H
#define __PUZZLE_SOLUTION_PERMUTER_FACTORY_H

#include <memory>

#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

std::unique_ptr<SolutionPermuter> CreateSolutionPermuter(
    EntryDescriptor* entry_descriptor, const std::vector<Solution::Cropper>& on_solution_with_class, Profiler* profiler);

}  // namespace puzzle

#endif  //  __PUZZLE_SOLUTION_PERMUTER_FACTORY_H
