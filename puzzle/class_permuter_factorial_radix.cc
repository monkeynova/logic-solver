#include "puzzle/class_permuter_factorial_radix.h"

namespace puzzle {

ClassPermuterFactorialRadix::Advancer::Advancer(
    const ClassPermuterFactorialRadix* permuter, ActiveSet active_set)
    : AdvancerBase(permuter, std::move(active_set)) {
  values_ = permuter->descriptor()->Values();
}

void ClassPermuterFactorialRadix::Advancer::Advance(int dist) {
  position_ += dist;
  if (position_ >= permutation_count()) {
    position_ = permutation_count();
    current_span_ = absl::Span<const int>();
  } else {
    int tmp = position_;
    for (size_t i = 0; i < permutation_size(); ++i) {
      current_[i] = values_[i];
    }
    for (size_t i = 0; tmp && i < permutation_size(); ++i) {
      int next = tmp % (permutation_size() - i);
      tmp /= (permutation_size() - i);
      std::swap(current_[i], current_[i + next]);
    }
  }
}

void ClassPermuterFactorialRadix::Advancer::Advance() { Advance(/*dist=*/1); }

}  // namespace puzzle
