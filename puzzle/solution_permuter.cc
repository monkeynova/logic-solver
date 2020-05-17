#include "puzzle/solution_permuter.h"

namespace puzzle {

SolutionPermuter::AdvancerBase::AdvancerBase(
    const EntryDescriptor* entry_descriptor)
    : mutable_solution_(entry_descriptor) {
  if (entry_descriptor != nullptr) {
    current_ = mutable_solution_.TestableSolution();
  }
}

}  // namespace puzzle
