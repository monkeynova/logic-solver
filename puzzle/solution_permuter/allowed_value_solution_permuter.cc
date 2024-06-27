#include "puzzle/solution_permuter/allowed_value_solution_permuter.h"

namespace puzzle {

AllowedValueSolutionPermuter::Advancer::Advancer(
    const AllowedValueSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : AdvancerBase(entry_descriptor), permuter_(permuter) {
  Advance();
}

void AllowedValueSolutionPermuter::Advancer::Advance() {
  set_position(position());
}

Position AllowedValueSolutionPermuter::Advancer::position() const {
  return {.position = 0, .count = 0};
}

AllowedValueSolutionPermuter::AllowedValueSolutionPermuter(
    const EntryDescriptor* e)
  : SolutionPermuter(e) {}

absl::StatusOr<bool> AllowedValueSolutionPermuter::AddFilter(
    SolutionFilter solution_filter) {
  return false;
}

absl::Status AllowedValueSolutionPermuter::PrepareCheap() {
  const EntryDescriptor* e = entry_descriptor();
  allowed_bv_.resize(e->num_classes());
  for (int i = 0; i < e->num_classes(); ++i) {
    int class_size = e->Class(i).size();
    int all_bv = (1 << class_size) - 1;
    allowed_bv_[i].resize(class_size, all_bv);
  }

  return absl::OkStatus();
}

absl::Status AllowedValueSolutionPermuter::PrepareFull() {
  return absl::OkStatus();
}

}  // namespace puzzle
