#include "puzzle/class_permuter.h"

namespace puzzle {
namespace internal {
  
template <enum ClassPermuterType T>
ClassPermuterImpl<T>::iterator::iterator(
    const ClassPermuterImpl<T>* permuter,
    ActiveSet active_set)
  : permuter_(permuter),
    active_set_(std::move(active_set)) {
  if (permuter_ != nullptr) {
    for (int i : permuter_->descriptor()->Values()) {
      current_.push_back(i);
    }
  }
  position_ = 0;
  index_.resize(current_.size());
  direction_.resize(current_.size());
  for (unsigned int i = 0; i < current_.size(); ++i) {
    index_[i] = i;
    direction_[i] = i == 0 ? 0 : -1;
  }
  next_from_ = current_.size() - 1;

  if (!active_set_.is_trivial()) {
    Advance(active_set_.ConsumeFalseBlock());
    CHECK(active_set_.ConsumeNext())
      << "ConsumeNext returned false after ConsumeFalseBlock";
  }
}

template <enum ClassPermuterType T>
void ClassPermuterImpl<T>::iterator::AdvanceWithSkip() {
  Advance(active_set_.ConsumeFalseBlock() + 1);
  CHECK(active_set_.ConsumeNext())
    << "ConsumeNext returned false after ConsumeFalseBlock";
}

template <enum ClassPermuterType T>
void ClassPermuterImpl<T>::iterator::Advance(int dist) {
  for (; dist > 0; --dist) Advance();
}
  
// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
template <>
void
ClassPermuterImpl<ClassPermuterType::kSteinhausJohnsonTrotter>
    ::iterator::Advance() {
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
    if (to == 0 || to == current_.size() - 1 ||
        index_[to + direction_[to]] > index_[to]) {
      direction_[to] = 0;
      int max = -1;
      for (int i = 0; i < current_.size(); ++i) {
        if (direction_[i] != 0 && index_[i] > max) {
          next_from_ = i;
          max = index_[i];
        }
      }
    } else {
      next_from_ = to;
    }
    if (index_[to] < current_.size() - 1) {
      for (int i = 0; i < current_.size(); ++i) {
        if (index_[i] > index_[to]) {
          if (i < to) {
            direction_[i] = 1;
          } else {
            direction_[i] = -1;
          }
        }
        if (index_[i] == current_.size() - 1) {
          next_from_ = i;
        }
      }
    }
  }
}

template <>
void
ClassPermuterImpl<ClassPermuterType::kFactorialRadix>::iterator::Advance() {
  ++position_;
  if (position_ >= permuter_->permutation_count()) {
    position_ = permuter_->permutation_count();
    current_.resize(0);
  } else {
    int tmp = position_;
    const std::vector<int>& values = permuter_->descriptor()->Values();
    for (unsigned int i = 0; i < current_.size(); ++i) {
      current_[i] = values[i];
    }
    for (unsigned int i = 0; tmp && i < current_.size(); ++i) {
      int next = tmp % (current_.size() - i);
      tmp /= (current_.size() - i);
      std::swap(current_[i], current_[i + next]);
    }
  }
}

// static
template <enum ClassPermuterType T>
double ClassPermuterImpl<T>::PermutationCount(const Descriptor* d) {
  if (d == nullptr) return 0;
  
  double ret = 1;
  int value_count = d->Values().size();
  for (int i = 2; i <= value_count; i++ ) {
    ret *= i;
  }
  return ret;
}

template class ClassPermuterImpl<ClassPermuterType::kSteinhausJohnsonTrotter>;
template class ClassPermuterImpl<ClassPermuterType::kFactorialRadix>;

}  // namespace internal
}  // namespace puzzle
