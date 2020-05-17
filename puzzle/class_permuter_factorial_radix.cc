#include "puzzle/class_permuter_factorial_radix.h"

namespace puzzle {

ClassPermuterFactorialRadix::Advancer::Advancer(
    const ClassPermuterFactorialRadix* permuter, ActiveSet active_set)
    : AdvancerBase(permuter, std::move(active_set)) {
  index_ = permuter_->descriptor()->Values();
}

void ClassPermuterFactorialRadix::Advancer::Advance(int dist) {
  position_ += dist;
  if (position_ >= permuter_->permutation_count()) {
    position_ = permuter_->permutation_count();
    current_.resize(0);
  } else {
    int tmp = position_;
    for (size_t i = 0; i < current_.size(); ++i) {
      current_[i] = index_[i];
    }
    for (size_t i = 0; tmp && i < current_.size(); ++i) {
      int next = tmp % (current_.size() - i);
      tmp /= (current_.size() - i);
      std::swap(current_[i], current_[i + next]);
    }
  }
}

void ClassPermuterFactorialRadix::Advancer::Advance() { Advance(/*dist=*/1); }

}  // namespace puzzle
