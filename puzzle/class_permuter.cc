#include "puzzle/class_permuter.h"

namespace puzzle {

ClassPermuter::AdvancerBase::AdvancerBase(const ClassPermuter* permuter)
    : position_(0),
      active_set_(ActiveSet::trivial()),
      active_set_it_(active_set_.GetIterator()),
      permutation_count_(permuter->permutation_count()),
      class_int_(permuter->class_int()) {}

ClassPermuter::AdvancerBase::AdvancerBase(const AdvancerBase& other)
    : position_(other.position_),
      active_set_(other.active_set_),
      active_set_it_(active_set_.GetIterator()),
      permutation_count_(other.permutation_count_),
      class_int_(other.class_int_) {
  active_set_it_.Advance(other.active_set_it_.offset());
}

bool ClassPermuter::AdvancerBase::WithActiveSet(const ActiveSet& other) {
  active_set_.Intersect(other);
  active_set_it_ = active_set_.GetIterator();
  active_set_it_.Advance(position_);
  if (active_set_it_.value()) {
    // Newly intersected active_set still points to an enabled offset.
    // Permtuer does not need to advance.
    return false;
  }
  AdvanceDelta(active_set_it_.run_size());
  active_set_it_.Advance(active_set_it_.run_size());
  DCHECK(active_set_it_.value())
      << "Value returned false after advancing past false block: it("
      << active_set_it_.offset() << " of " << active_set_it_.total()
      << "): " << active_set_.DebugValues();
  // Previous location was no longer valued and we advanced.
  return true;
}

void ClassPermuter::AdvancerBase::AdvanceWithSkip() {
  DCHECK(active_set_it_.value());
  int delta = 1;
  active_set_it_.Advance(delta);
  if (!active_set_it_.value()) {
    DCHECK(active_set_it_.run_size() > 0) << "0 length false run";
    delta += active_set_it_.run_size();
    active_set_it_.Advance(active_set_it_.run_size());
  }
  DCHECK(active_set_it_.value())
      << "Value returned false after advancing past false block: it("
      << active_set_it_.offset() << " of " << active_set_it_.total()
      << "): " << active_set_.DebugValues();
  AdvanceDelta(delta);
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
