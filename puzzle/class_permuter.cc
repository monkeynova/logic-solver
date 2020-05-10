#include "puzzle/class_permuter.h"

namespace puzzle {
namespace internal {

template <>
void ClassPermuterImpl<
    ClassPermuterType::kSteinhausJohnsonTrotter>::iterator::InitIndex() {
  index_.resize(current_.size());
  direction_.resize(current_.size());
  for (size_t i = 0; i < current_.size(); ++i) {
    index_[i] = i;
    direction_[i] = i == 0 ? 0 : -1;
  }
  next_from_ = current_.size() - 1;
}

template <>
void ClassPermuterImpl<
    ClassPermuterType::kFactorialRadix>::iterator::InitIndex() {
  if (permuter_ != nullptr) {
    index_ = permuter_->descriptor()->Values();
  }
}

template <>
void ClassPermuterImpl<
    ClassPermuterType::kFactorialRadixDeleteTracking>::iterator::InitIndex() {
  if (permuter_ != nullptr) {
    index_ = permuter_->descriptor()->Values();
  }
}

template <enum ClassPermuterType T>
ClassPermuterImpl<T>::iterator::iterator(const ClassPermuterImpl<T>* permuter,
                                         ActiveSet active_set)
    : permuter_(permuter), active_set_(std::move(active_set)) {
  if (permuter_ != nullptr) {
    for (int i : permuter_->descriptor()->Values()) {
      current_.push_back(i);
    }
  }
  position_ = 0;
  InitIndex();

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

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
template <>
void ClassPermuterImpl<
    ClassPermuterType::kSteinhausJohnsonTrotter>::iterator::Advance() {
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

template <>
void ClassPermuterImpl<
    ClassPermuterType::kSteinhausJohnsonTrotter>::iterator::Advance(int dist) {
  for (; dist > 0; --dist) Advance();
}

template <>
void ClassPermuterImpl<ClassPermuterType::kSteinhausJohnsonTrotter>::iterator::
    Advance(ValueSkip value_skip) {
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

template <>
void ClassPermuterImpl<ClassPermuterType::kFactorialRadix>::iterator::Advance(
    int dist) {
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

template <>
void ClassPermuterImpl<
    ClassPermuterType::kFactorialRadix>::iterator::Advance() {
  Advance(/*dist=*/1);
}

template <>
void ClassPermuterImpl<ClassPermuterType::kFactorialRadix>::iterator::Advance(
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

template <>
void ClassPermuterImpl<ClassPermuterType::kFactorialRadixDeleteTracking>::
    iterator::Advance(int dist) {
  position_ += dist;
  if (position_ >= permuter_->permutation_count()) {
    position_ = permuter_->permutation_count();
    current_.resize(0);
  } else {
    int mod = current_.size();
    int div = permuter_->permutation_count() / mod;
    std::vector<int> deleted(index_.size(), 0);
    for (size_t i = 0; i < current_.size() - 1; ++i) {
      int next = (position_ / div) % mod;
      for (int j = 0; j <= next; ++j) {
        if (deleted[j]) ++next;
      }
      DCHECK_LT(next, index_.size());
      current_[i] = index_[next];
      deleted[next] = 1;
      --mod;
      div /= mod;
    }
    int next = -1;
    for (int j = 0; j < deleted.size(); ++j) {
      if (!deleted[j]) {
        next = j;
        break;
      }
    }
    DCHECK_GE(next, 0);
    current_[current_.size() - 1] = index_[next];
  }
}

template <>
void ClassPermuterImpl<
    ClassPermuterType::kFactorialRadixDeleteTracking>::iterator::Advance() {
  Advance(/*dist=*/1);
}

template <>
void ClassPermuterImpl<ClassPermuterType::kFactorialRadixDeleteTracking>::
    iterator::Advance(ValueSkip value_skip) {
  int value = current_[value_skip.value_index];
  while (!current_.empty() && current_[value_skip.value_index] == value) {
    int div = 1;
    for (int i = 1; i <= current_.size() - value_skip.value_index - 1; ++i) {
      div *= i;
    }
    int delta = div - (position_ % div);
    if (!active_set_.is_trivial()) {
      active_set_.DiscardBlock(delta);
      delta += active_set_.ConsumeFalseBlock();
    }
    Advance(/*dist=*/delta);
  }
}

// static
template <enum ClassPermuterType T>
double ClassPermuterImpl<T>::PermutationCount(const Descriptor* d) {
  if (d == nullptr) return 0;

  double ret = 1;
  int value_count = d->Values().size();
  for (int i = 2; i <= value_count; i++) {
    ret *= i;
  }
  return ret;
}

template <enum ClassPermuterType T>
std::string ClassPermuterImpl<T>::DebugString() const {
  return absl::StrJoin(
      *this, ", ",
      [](std::string* out, const typename iterator::StorageVector& v) {
        absl::StrAppend(out, "{", absl::StrJoin(v, ","), "}");
      });
}

template class ClassPermuterImpl<ClassPermuterType::kSteinhausJohnsonTrotter>;
template class ClassPermuterImpl<ClassPermuterType::kFactorialRadix>;
template class ClassPermuterImpl<
    ClassPermuterType::kFactorialRadixDeleteTracking>;

}  // namespace internal
}  // namespace puzzle
