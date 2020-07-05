#include "puzzle/class_permuter_factorial_radix.h"

namespace puzzle {

template <int kStorageSize>
ClassPermuterFactorialRadix<kStorageSize>::Advancer::Advancer(
    const ClassPermuterFactorialRadix* permuter)
    : Base(permuter) {}

template <int kStorageSize>
void ClassPermuterFactorialRadix<kStorageSize>::Advancer::AdvanceDelta(
    int dist) {
  Base::position_ += dist;
  if (Base::position_ >= Base::permutation_count()) {
    Base::position_ = Base::permutation_count();
    Base::set_current(absl::Span<const int>());
  } else {
    int tmp = Base::position_;
    for (size_t i = 0; i < kStorageSize; ++i) {
      Base::current_[i] = i;
    }
    for (size_t i = 0; tmp && i < kStorageSize; ++i) {
      int next = tmp % (kStorageSize - i);
      tmp /= (kStorageSize - i);
      std::swap(Base::current_[i], Base::current_[i + next]);
    }
  }
}

template <int kStorageSize>
void ClassPermuterFactorialRadix<kStorageSize>::Advancer::Advance() {
  AdvanceDelta(/*dist=*/1);
}

static constexpr int kMaxStorageSize = 32;

template <int kStorageSize>
static std::unique_ptr<ClassPermuter> MakeSizedInstance(int permutation_size,
                                                        int class_int) {
  if (permutation_size == kStorageSize) {
    return absl::make_unique<ClassPermuterFactorialRadix<kStorageSize>>(
        permutation_size, class_int);
  }
  return MakeSizedInstance<kStorageSize - 1>(permutation_size, class_int);
}

template <>
std::unique_ptr<ClassPermuter> MakeSizedInstance<0>(int permutation_size,
                                                    int class_int) {
  return nullptr;
}

std::unique_ptr<ClassPermuter> MakeClassPermuterFactorialRadix::operator()(
    int permutation_size, int class_int) {
  return MakeSizedInstance<kMaxStorageSize>(permutation_size, class_int);
}

}  // namespace puzzle
