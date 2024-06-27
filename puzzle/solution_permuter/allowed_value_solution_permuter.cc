#include "puzzle/solution_permuter/allowed_value_solution_permuter.h"

namespace puzzle {

namespace {

class AllowedValueAdvancer final : public SolutionPermuter::AdvancerBase {
 public:
  explicit AllowedValueAdvancer(const AllowedValueSolutionPermuter* permuter,
                                const EntryDescriptor* entry_descriptor);
  AllowedValueAdvancer(const AllowedValueAdvancer&) = delete;

  AllowedValueAdvancer& operator=(const AllowedValueAdvancer&) = delete;
  Position position() const;

 private:
  void Advance() override;
  const AllowedValueSolutionPermuter* permuter_;
};

AllowedValueAdvancer::AllowedValueAdvancer(
    const AllowedValueSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : AdvancerBase(entry_descriptor), permuter_(permuter) {
  Advance();
}

void AllowedValueAdvancer::Advance() {
  set_position(position());
  set_done();
}

Position AllowedValueAdvancer::position() const {
  return {.position = 0, .count = 0};
}

}  // namespace


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
  return absl::UnimplementedError(
      "AllowedValueSolutionPermuter not implemented");
}

double AllowedValueSolutionPermuter::permutation_count() const {
  double permutations_count = 1;
  const EntryDescriptor* e = entry_descriptor();
  for (int i = 0; i < e->num_classes(); ++i) {
    permutations_count *= e->Class(i).size();
  }
  return permutations_count;
}

AllowedValueSolutionPermuter::iterator AllowedValueSolutionPermuter::begin() const {
  return iterator(absl::make_unique<AllowedValueAdvancer>(this, entry_descriptor()));
}

}  // namespace puzzle
