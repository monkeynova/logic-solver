#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"

#include "absl/synchronization/mutex.h"

namespace puzzle {

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

ClassPermuterFactorialRadixDeleteTracking::Advancer::Advancer(
    const ClassPermuterFactorialRadixDeleteTracking* permuter,
    ActiveSet active_set)
    : AdvancerBase(permuter, std::move(active_set)) {
  values_ = current_;
  CHECK_LT(permutation_size(), 20)
      << "Permutation indexes use a memory buffer of size N * 2^N";
  radix_index_to_raw_index_ = GetRadixIndexToRawIndex(permutation_size());
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance(int dist) {
  position_ += dist;
  if (position_ >= permutation_count()) {
    position_ = permutation_count();
    current_span_ = absl::Span<const int>();
  } else {
    int mod = permutation_size();
    int div = permutation_count() / mod;
    int deleted = 0;
    DCHECK_LT(permutation_size(), 32)
        << "Permutation indexes must be useable as a bit vector";
    for (size_t i = 0; i < permutation_size() - 1; ++i) {
      const int next =
          (*radix_index_to_raw_index_)[(position_ / div) % mod][deleted];
      DCHECK_LT(next, permutation_size());
      current_[i] = values_[next];
      deleted |= (1 << next);
      --mod;
      div /= mod;
    }
    const int next = (*radix_index_to_raw_index_)[0][deleted];
    DCHECK_GE(next, 0);
    current_[permutation_size() - 1] = values_[next];
  }
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance() {
  Advance(/*dist=*/1);
}

static int factorial(int n) {
  int ret = 1;
  for (int i = 2; i <= n; ++i) {
    ret *= i;
  }
  return ret;
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance(
    ValueSkip value_skip) {
  int value = current_[value_skip.value_index];
  int div = factorial(permutation_size() - value_skip.value_index - 1);
  int delta = div - (position_ % div);
  do {
    if (!active_set_.is_trivial()) {
      if (!active_set_.DiscardBlock(delta)) {
        delta += active_set_.ConsumeFalseBlock() + 1;
        CHECK(active_set_.ConsumeNext())
            << "ConsumeNext returned false after ConsumeFalseBlock";
      }
    }
    Advance(/*dist=*/delta);
    if (active_set_.is_trivial()) {
      delta = div;
    } else {
      delta = div - (position_ % div);
    }
  } while (!current_span_.empty() && current_[value_skip.value_index] == value);
}

}  // namespace puzzle
