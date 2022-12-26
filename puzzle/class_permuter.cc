#include "puzzle/class_permuter.h"

namespace puzzle {

ClassPermuter::AdvancerBase::AdvancerBase(const ClassPermuter* permuter)
    : position_(0),
      active_set_(&ActiveSet::trivial()),
      active_set_it_(active_set_->GetIterator()),
      active_set_owned_(false),
      permutation_count_(permuter->permutation_count()),
      class_int_(permuter->class_int()) {}

ClassPermuter::AdvancerBase::AdvancerBase(const AdvancerBase& other)
    : position_(other.position_),
      active_set_(other.active_set_),
      active_set_it_(active_set_->GetIterator()),
      active_set_owned_(false),
      permutation_count_(other.permutation_count_),
      class_int_(other.class_int_) {
  active_set_it_.Advance(other.active_set_it_.offset());
}

bool ClassPermuter::AdvancerBase::WithActiveSet(const ActiveSet& other) {
  if (active_set_->is_trivial()) {
    active_set_ = &other;
  } else {
    active_set_ = new ActiveSet(active_set_->Intersection(other));
    active_set_owned_ = true;
  }
  active_set_it_ = active_set_->GetIterator();
  active_set_it_.Advance(position_);
  if (active_set_it_.value()) {
    // Newly intersected active_set still points to an enabled offset.
    // Permuter does not need to advance.
    return false;
  }
  AdvanceDelta(active_set_it_.RunSize());
  active_set_it_.Advance(active_set_it_.RunSize());
  DCHECK(active_set_it_.value())
      << "Value returned false after advancing past false block: it("
      << active_set_it_.offset() << " of " << active_set_it_.total()
      << "): " << active_set_->DebugValues();
  // Previous location was no longer valued and we advanced.
  return true;
}

void ClassPermuter::AdvancerBase::AdvanceDeltaWithSkip(int delta) {
  DCHECK(active_set_it_.value());
  active_set_it_.Advance(delta);
  if (!active_set_it_.value()) {
    delta += active_set_it_.RunSize();
    active_set_it_.Advance(active_set_it_.RunSize());
  }
  DCHECK(active_set_it_.value() ||
         active_set_it_.offset() == active_set_it_.total())
      << "Value returned false after advancing past false block: it("
      << active_set_it_.offset() << " of " << active_set_it_.total()
      << "): " << active_set_->DebugValues();
  AdvanceDelta(delta);
}

// static
int ClassPermuter::PermutationCount(int permutation_size) {
  int ret = 1;
  int value_count = permutation_size;
  for (int i = 2; i <= value_count; i++) {
    ret *= i;
  }
  return ret;
}

std::string ClassPermuter::DebugString() const {
  std::string ret;
  for (const absl::Span<const int> v : *this) {
    if (!ret.empty()) absl::StrAppend(&ret, ",");
    absl::StrAppend(&ret, "{", absl::StrJoin(v, ","), "}");
  }
  return ret;
}

}  // namespace puzzle
