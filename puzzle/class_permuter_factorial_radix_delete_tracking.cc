#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"

#include "absl/synchronization/mutex.h"

namespace puzzle {

static constexpr int kMaxStorageSize = 20;

// Global container/cache for `radix_index_to_raw_index_`. Keyed on the  number
// of items in the permutation.
// This is a non-trivial amount of memory to use and failure to share the
// memory between ClassPermuters causes memory pressure on the CPU cache which
// in turn is very present in profiling.
static absl::Mutex max_pos_to_radix_index_to_raw_index_lock_;
static std::vector<std::unique_ptr<RadixIndexToRawIndex>>
    max_pos_to_radix_index_to_raw_index_
        ABSL_GUARDED_BY(max_pos_to_radix_index_to_raw_index_lock_);

static int ComputeRadixIndexToRawIndex(int position, int delete_bit_vector) {
  for (int j = 0; j <= position; ++j) {
    if (delete_bit_vector & (1 << j)) ++position;
  }
  return position;
}

static RadixIndexToRawIndex ComputeAllRadixIndexToRawIndex(int max_pos) {
  RadixIndexToRawIndex ret;
  ret.resize(max_pos);
  for (int position = 0; position < max_pos; ++position) {
    ret[position].resize(1 << max_pos, 0);
    for (int bv = 0; bv < (1 << max_pos); ++bv) {
      ret[position][bv] = ComputeRadixIndexToRawIndex(position, bv);
    }
  }
  return ret;
}

static RadixIndexToRawIndex* GetRadixIndexToRawIndex(int max_pos) {
  absl::MutexLock l(&max_pos_to_radix_index_to_raw_index_lock_);
  if (max_pos_to_radix_index_to_raw_index_.size() < max_pos + 1) {
    max_pos_to_radix_index_to_raw_index_.resize(max_pos + 1);
  }
  if (max_pos_to_radix_index_to_raw_index_[max_pos] == nullptr) {
    max_pos_to_radix_index_to_raw_index_[max_pos] =
        absl::make_unique<RadixIndexToRawIndex>(
            ComputeAllRadixIndexToRawIndex(max_pos));
  }

  return max_pos_to_radix_index_to_raw_index_[max_pos].get();
}

template <int kStorageSize>
ClassPermuterFactorialRadixDeleteTracking<kStorageSize>::Advancer::Advancer(
    const ClassPermuterFactorialRadixDeleteTracking* permuter)
    : Base(permuter) {
  memcpy(values_, Base::current_, sizeof(values_));
  DCHECK_LT(kStorageSize, kMaxStorageSize)
      << "Permutation indexes use a memory buffer of size N * 2^N";
  radix_index_to_raw_index_ = GetRadixIndexToRawIndex(kStorageSize);
}

template <int kStorageSize>
void ClassPermuterFactorialRadixDeleteTracking<
    kStorageSize>::Advancer::AdvanceDelta(int dist) {
  Base::position_ += dist;
  if (Base::position_ >= Base::permutation_count()) {
    Base::position_ = Base::permutation_count();
    Base::current_span_ = absl::Span<const int>();
  } else {
    int mod = kStorageSize;
    int div = Base::permutation_count() / mod;
    int deleted = 0;
    DCHECK_LT(kStorageSize, kMaxStorageSize)
        << "Permutation indexes must be useable as a bit vector";
    for (size_t i = 0; i < kStorageSize - 1; ++i) {
      const int next =
          (*radix_index_to_raw_index_)[(Base::position_ / div) % mod][deleted];
      DCHECK_LT(next, kStorageSize);
      Base::current_[i] = values_[next];
      deleted |= (1 << next);
      --mod;
      div /= mod;
    }
    const int next = (*radix_index_to_raw_index_)[0][deleted];
    DCHECK_GE(next, 0);
    Base::current_[kStorageSize - 1] = values_[next];
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
  int delta = div - (Base::position_ % div);
  if (Base::active_set_.is_trivial()) {
    do {
      AdvanceDelta(/*dist=*/delta);
      delta = div;
    } while (!Base::current_span_.empty() &&
	     Base::current_[value_skip.value_index] == value);
  } else {
    do {
      Base::active_set_it_.Advance(delta);
      if (!Base::active_set_it_.value()) {
	delta += Base::active_set_it_.RunSize();
	Base::active_set_it_.Advance(Base::active_set_it_.RunSize());
	DCHECK(Base::active_set_it_.value())
	  << "Value returned false after advancing past false block: it("
	  << Base::active_set_it_.offset() << " of "
	  << Base::active_set_it_.total()
	  << "): " << Base::active_set_.DebugValues();
      }
      AdvanceDelta(/*dist=*/delta);
      delta = div - (Base::position_ % div);
    } while (!Base::current_span_.empty() &&
	     Base::current_[value_skip.value_index] == value);
  }
}

template <int kStorageSize>
static std::unique_ptr<ClassPermuter> MakeSizedInstance(int permutation_size,
                                                        const Descriptor* d,
                                                        int class_int) {
  if (permutation_size == kStorageSize) {
    return absl::make_unique<
        ClassPermuterFactorialRadixDeleteTracking<kStorageSize>>(d, class_int);
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
MakeClassPermuterFactorialRadixDeleteTracking::operator()(const Descriptor* d,
                                                          int class_int) {
  return MakeSizedInstance<kMaxStorageSize>(d->Values().size(), d, class_int);
}

}  // namespace puzzle
