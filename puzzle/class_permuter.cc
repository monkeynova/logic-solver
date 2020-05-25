#include "puzzle/class_permuter.h"

namespace puzzle {

ClassPermuter::AdvancerBase::AdvancerBase(const ClassPermuter* permuter)
    : position_(0),
      active_set_(ActiveSet::trivial()),
      permutation_count_(permuter->permutation_count()),
      class_int_(permuter->class_int()) {}

bool ClassPermuter::AdvancerBase::WithActiveSet(ActiveSet other) {
  bool new_value_matches = other.DiscardBlock(position_ + 1);
  active_set_.Intersect(other);
  if (!new_value_matches) {
    AdvanceDelta(active_set_.ConsumeFalseBlock() + 1);
    CHECK(active_set_.ConsumeNext())
        << "ConsumeNext returned false after ConsumeFalseBlock";
  }
  // Return whether or not the iterator was advanced because it was
  // invalidated.
  return !new_value_matches;
}

void ClassPermuter::AdvancerBase::AdvanceWithSkip() {
  AdvanceDelta(active_set_.ConsumeFalseBlock() + 1);
  CHECK(active_set_.ConsumeNext())
      << "ConsumeNext returned false after ConsumeFalseBlock";
}

// static
int ClassPermuter::PermutationCount(const Descriptor* d) {
  if (d == nullptr) return 0;

  int ret = 1;
  int value_count = d->Values().size();
  for (int i = 2; i <= value_count; i++) {
    ret *= i;
  }
  return ret;
}

std::string ClassPermuter::DebugString() const {
  return absl::StrJoin(*this, ", ",
                       [](std::string* out, absl::Span<const int> v) {
                         absl::StrAppend(out, "{", absl::StrJoin(v, ","), "}");
                       });
}

}  // namespace puzzle
