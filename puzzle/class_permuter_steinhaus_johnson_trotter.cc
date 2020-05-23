#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

namespace puzzle {

template <int kStorageSize>
ClassPermuterSteinhausJohnsonTrotter<kStorageSize>::Advancer::Advancer(
    const ClassPermuterSteinhausJohnsonTrotter* permuter, ActiveSet active_set)
    : Base(permuter, std::move(active_set)) {
  for (size_t i = 0; i < kStorageSize; ++i) {
    index_[i] = i;
    direction_[i] = i == 0 ? 0 : -1;
  }
  next_from_ = kStorageSize - 1;
}

// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
template <int kStorageSize>
void ClassPermuterSteinhausJohnsonTrotter<kStorageSize>::Advancer::Advance() {
  ++Base::position_;
  if (Base::position_ >= Base::permutation_count()) {
    Base::position_ = Base::permutation_count();
    Base::current_span_ = absl::Span<const int>();
  } else {
    int from = next_from_;
    int to = from + direction_[from];
    std::swap(Base::current_[from], Base::current_[to]);
    std::swap(direction_[from], direction_[to]);
    std::swap(index_[from], index_[to]);
    if (to == 0 || to == kStorageSize - 1 ||
        index_[to + direction_[to]] > index_[to]) {
      direction_[to] = 0;
      int max = -1;
      for (size_t i = 0; i < kStorageSize; ++i) {
        if (direction_[i] != 0 && index_[i] > max) {
          next_from_ = i;
          max = index_[i];
        }
      }
    } else {
      next_from_ = to;
    }
    if (index_[to] < kStorageSize - 1) {
      for (size_t i = 0; i < kStorageSize; ++i) {
        if (index_[i] > index_[to]) {
          if (static_cast<int>(i) < to) {
            direction_[i] = 1;
          } else {
            direction_[i] = -1;
          }
        }
        if (index_[i] == kStorageSize - 1) {
          next_from_ = i;
        }
      }
    }
  }
}

template <int kStorageSize>
void ClassPermuterSteinhausJohnsonTrotter<kStorageSize>::Advancer::AdvanceDelta(
    int dist) {
  for (; dist > 0; --dist) Advance();
}

static constexpr int kMaxStorageSize = 32;

template <int kStorageSize>
static std::unique_ptr<ClassPermuter> MakeSizedInstance(int permutation_size,
                                                        const Descriptor* d,
                                                        int class_int) {
  if (permutation_size == kStorageSize) {
    return absl::make_unique<
        ClassPermuterSteinhausJohnsonTrotter<kStorageSize>>(d, class_int);
  }
  return MakeSizedInstance<kStorageSize - 1>(permutation_size, d, class_int);
}

template <>
std::unique_ptr<ClassPermuter> MakeSizedInstance<0>(int permutation_size,
                                                    const Descriptor* d,
                                                    int class_int) {
  return nullptr;
}

std::unique_ptr<ClassPermuter>
MakeClassPermuterSteinhausJohnsonTrotter::operator()(const Descriptor* d,
                                                     int class_int) {
  return MakeSizedInstance<kMaxStorageSize>(d->Values().size(), d, class_int);
}

}  // namespace puzzle
