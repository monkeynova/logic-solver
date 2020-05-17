#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

namespace puzzle {

ClassPermuterSteinhausJohnsonTrotter::Advancer::Advancer(
    const ClassPermuterSteinhausJohnsonTrotter* permuter, ActiveSet active_set)
    : AdvancerBase(permuter, std::move(active_set)) {
  index_.resize(current_.size());
  direction_.resize(current_.size());
  for (size_t i = 0; i < current_.size(); ++i) {
    index_[i] = i;
    direction_[i] = i == 0 ? 0 : -1;
  }
  next_from_ = current_.size() - 1;
}

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
void ClassPermuterSteinhausJohnsonTrotter::Advancer::Advance() {
  ++position_;
  if (position_ >= permuter_->permutation_count()) {
    position_ = permuter_->permutation_count();
    current_.resize(0);
  } else {
    int from = next_from_;
    int to = from + direction_[from];
    std::swap(current_[from], current_[to]);
    std::swap(direction_[from], direction_[to]);
    std::swap(index_[from], index_[to]);
    if (to == 0 || to == static_cast<int>(current_.size()) - 1 ||
        index_[to + direction_[to]] > index_[to]) {
      direction_[to] = 0;
      int max = -1;
      for (size_t i = 0; i < current_.size(); ++i) {
        if (direction_[i] != 0 && index_[i] > max) {
          next_from_ = i;
          max = index_[i];
        }
      }
    } else {
      next_from_ = to;
    }
    if (index_[to] < static_cast<int>(current_.size()) - 1) {
      for (size_t i = 0; i < current_.size(); ++i) {
        if (index_[i] > index_[to]) {
          if (static_cast<int>(i) < to) {
            direction_[i] = 1;
          } else {
            direction_[i] = -1;
          }
        }
        if (index_[i] == static_cast<int>(current_.size()) - 1) {
          next_from_ = i;
        }
      }
    }
  }
}

void ClassPermuterSteinhausJohnsonTrotter::Advancer::Advance(int dist) {
  for (; dist > 0; --dist) Advance();
}

void ClassPermuterSteinhausJohnsonTrotter::Advancer::Advance(
    ValueSkip value_skip) {
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

}  // namespace puzzle
