#include "puzzle/class_permuter_factorial_radix.h"

namespace puzzle {

template <int kStorageSize>
ClassPermuterFactorialRadix<kStorageSize>::Advancer::Advancer(
    const ClassPermuterFactorialRadix* permuter, ActiveSet active_set)
    : Base(permuter, std::move(active_set)) {
  DCHECK_EQ(kStorageSize, permuter->descriptor()->Values().size());
  memcpy(values_, permuter->descriptor()->Values().data(), sizeof(values_));
}

template <int kStorageSize>
void ClassPermuterFactorialRadix<kStorageSize>::Advancer::AdvanceDelta(
    int dist) {
  Base::position_ += dist;
  if (Base::position_ >= Base::permutation_count()) {
    Base::position_ = Base::permutation_count();
    Base::current_span_ = absl::Span<const int>();
  } else {
    int tmp = Base::position_;
    for (size_t i = 0; i < kStorageSize; ++i) {
      Base::current_[i] = values_[i];
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
                                                        const Descriptor* d,
                                                        int class_int) {
  if (permutation_size == kStorageSize) {
    return absl::make_unique<ClassPermuterFactorialRadix<kStorageSize>>(
        d, class_int);
  }
  return MakeSizedInstance<kStorageSize - 1>(permutation_size, d, class_int);
}

template <>
std::unique_ptr<ClassPermuter> MakeSizedInstance<0>(int permutation_size,
                                                    const Descriptor* d,
                                                    int class_int) {
  return nullptr;
}

std::unique_ptr<ClassPermuter> MakeClassPermuterFactorialRadix::operator()(
    const Descriptor* d, int class_int) {
  return MakeSizedInstance<kMaxStorageSize>(d->Values().size(), d, class_int);
}

}  // namespace puzzle
