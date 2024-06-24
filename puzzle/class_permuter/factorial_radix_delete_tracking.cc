#include "puzzle/class_permuter/factorial_radix_delete_tracking.h"

#include "absl/synchronization/mutex.h"

namespace puzzle {

static constexpr int kMaxStorageSize = 20;

namespace {

// Contains a map from a radix index position and a bit vector marked with
// previously selected values from index_ in a permutation to the index for
// the correspondingly selected value in index_.
// This is a lookup-table for the function RadixIndexToRawIndex::ComputeValue
// which is needed in the innermost loop of permutation calculation.
template <int kMaxPos>
class RadixIndexToRawIndex {
 public:
  static int Get(int position, int bit_vector) {
    static RadixIndexToRawIndex<kMaxPos> singleton;
    return singleton.data_[bit_vector * kMaxPos + position];
  }

 private:
  RadixIndexToRawIndex() { Initialize(); }

  void Initialize();
  static int ComputeValue(int position, int delete_bit_vector);

  void Set(int position, int bit_vector, int value) {
    data_[bit_vector * kMaxPos + position] = value;
  }

  int data_[kMaxPos << kMaxPos];
};

// static
template <int kMaxPos>
int RadixIndexToRawIndex<kMaxPos>::ComputeValue(int position,
                                                int delete_bit_vector) {
  CHECK_LT(position, kMaxPos);
  CHECK_EQ(delete_bit_vector & (0xffffffff << kMaxPos), 0);
  for (int j = 0; j <= position; ++j) {
    if (delete_bit_vector & (1 << j)) ++position;
  }
  return position;
}

template <int kMaxPos>
void RadixIndexToRawIndex<kMaxPos>::Initialize() {
  for (int position = 0; position < kMaxPos; ++position) {
    for (int bv = 0; bv < (1 << kMaxPos); ++bv) {
      Set(position, bv, ComputeValue(position, bv));
    }
  }
}

}  // namespace

template <int kStorageSize>
ClassPermuterFactorialRadixDeleteTracking<kStorageSize>::Advancer::Advancer(
    const ClassPermuterFactorialRadixDeleteTracking* permuter)
    : Base(permuter) {
  static_assert(kStorageSize <=  kMaxStorageSize,
                "Permutation indexes use a memory buffer of size N * 2^N");
}

template <int kStorageSize>
void ClassPermuterFactorialRadixDeleteTracking<
    kStorageSize>::Advancer::AdvanceDelta(int dist) {
  Base::position_ += dist;
  if (Base::position_ >= Base::permutation_count()) {
    Base::position_ = Base::permutation_count();
    Base::set_current(absl::Span<const int>());
  } else {
    int mod = kStorageSize;
    int div = Base::permutation_count() / mod;
    int deleted = 0;
    static_assert(kStorageSize <=  kMaxStorageSize,
                  "Permutation indexes must be useable as a bit vector");
    for (size_t i = 0; i < kStorageSize - 1; ++i) {
      const int next = RadixIndexToRawIndex<kStorageSize>::Get(
          (Base::position_ / div) % mod, deleted);
      DCHECK_LT(next, kStorageSize);
      Base::current_[i] = next;
      deleted |= (1 << next);
      --mod;
      div /= mod;
    }
    const int next = RadixIndexToRawIndex<kStorageSize>::Get(0, deleted);
    DCHECK_GE(next, 0);
    Base::current_[kStorageSize - 1] = next;
  }
}

template <int kStorageSize>
void ClassPermuterFactorialRadixDeleteTracking<
    kStorageSize>::Advancer::Advance() {
  AdvanceDelta(/*dist=*/1);
}

static int factorial(int n) {
  int ret = 1;
  for (int i = 2; i <= n; ++i) {
    ret *= i;
  }
  return ret;
}

template <int kStorageSize>
void ClassPermuterFactorialRadixDeleteTracking<
    kStorageSize>::Advancer::AdvanceSkip(ValueSkip value_skip) {
  int value = Base::current_[value_skip.value_index];
  int div = factorial(kStorageSize - value_skip.value_index - 1);
  auto still_on_value = [&]() {
    return !Base::done() && Base::current_[value_skip.value_index] == value;
  };
  if (Base::active_set_->is_trivial()) {
    int delta = div - (Base::position_ % div);
    do {
      AdvanceDelta(/*dist=*/delta);
      delta = div;
    } while (still_on_value());
  } else {
    do {
      int delta = div - (Base::position_ % div);
      Base::active_set_it_.Advance(delta);
      if (!Base::active_set_it_.value()) {
        int next_delta = Base::active_set_it_.RunSize();
        delta += next_delta;
        Base::active_set_it_.Advance(next_delta);
        DCHECK(Base::active_set_it_.value())
            << "Value returned false after advancing past false block: it("
            << Base::active_set_it_.offset() << " of "
            << Base::active_set_it_.total()
            << "): " << Base::active_set_->DebugValues();
      }
      AdvanceDelta(/*dist=*/delta);
    } while (still_on_value());
  }
}

template <int kStorageSize>
static std::unique_ptr<ClassPermuter> MakeSizedInstance(int permutation_size,
                                                        int class_int) {
  if (permutation_size == kStorageSize) {
    return absl::make_unique<
        ClassPermuterFactorialRadixDeleteTracking<kStorageSize>>(
        permutation_size, class_int);
  }
  return MakeSizedInstance<kStorageSize - 1>(permutation_size, class_int);
}

template <>
std::unique_ptr<ClassPermuter> MakeSizedInstance<0>(int permutation_size,
                                                    int class_int) {
  return nullptr;
}

std::unique_ptr<ClassPermuter>
MakeClassPermuterFactorialRadixDeleteTracking::operator()(int permutation_size,
                                                          int class_int) {
  return MakeSizedInstance<kMaxStorageSize>(permutation_size, class_int);
}

}  // namespace puzzle
