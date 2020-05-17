#include "puzzle/class_permuter.h"

namespace puzzle {

ClassPermuter::AdvancerBase::AdvancerBase(const ClassPermuter* permuter,
                                          ActiveSet active_set)
  : current_(permuter->descriptor()->Values()),
    position_(0),
    active_set_(std::move(active_set)),
    permutation_size_(permuter->descriptor()->Values().size()),
    permutation_count_(permuter->permutation_count()),
    class_int_(permuter->class_int()) {
}

void ClassPermuter::AdvancerBase::Prepare() {
  if (!active_set_.is_trivial()) {
    Advance(active_set_.ConsumeFalseBlock());
    CHECK(active_set_.ConsumeNext())
        << "ConsumeNext returned false after ConsumeFalseBlock";
  }
}

void ClassPermuter::AdvancerBase::AdvanceWithSkip() {
  Advance(active_set_.ConsumeFalseBlock() + 1);
  CHECK(active_set_.ConsumeNext())
      << "ConsumeNext returned false after ConsumeFalseBlock";
}

void ClassPermuter::AdvancerBase::Advance(ValueSkip value_skip) {
  int value = current_[value_skip.value_index];
  if (active_set_.is_trivial()) {
    while (!current_.empty() && current_[value_skip.value_index] == value) {
      Advance();
    }
  } else {
    while (!current_.empty() && current_[value_skip.value_index] == value) {
      AdvanceWithSkip();
    }
  }
}

// static
double ClassPermuter::PermutationCount(const Descriptor* d) {
  if (d == nullptr) return 0;

  double ret = 1;
  int value_count = d->Values().size();
  for (int i = 2; i <= value_count; i++) {
    ret *= i;
  }
  return ret;
}

std::string ClassPermuter::DebugString() const {
  return absl::StrJoin(
      *this, ", ",
      [](std::string* out, const typename iterator::StorageVector& v) {
        absl::StrAppend(out, "{", absl::StrJoin(v, ","), "}");
      });
}

}  // namespace puzzle
