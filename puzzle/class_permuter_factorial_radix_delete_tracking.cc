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
  values_ = permuter_->descriptor()->Values();
  radix_index_to_raw_index_ = GetRadixIndexToRawIndex(values_.size());
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance(int dist) {
  position_ += dist;
  if (position_ >= permuter_->permutation_count()) {
    position_ = permuter_->permutation_count();
    current_.resize(0);
  } else {
    int mod = current_.size();
    int div = permuter_->permutation_count() / mod;
    int deleted = 0;
    DCHECK_LT(current_.size(), 32)
        << "Permutation indexes must be useable as a bit vector";
    for (size_t i = 0; i < current_.size() - 1; ++i) {
      const int next =
          (*radix_index_to_raw_index_)[(position_ / div) % mod][deleted];
      DCHECK_LT(next, values_.size());
      current_[i] = values_[next];
      deleted |= (1 << next);
      --mod;
      div /= mod;
    }
    const int next = (*radix_index_to_raw_index_)[0][deleted];
    DCHECK_GE(next, 0);
    current_[current_.size() - 1] = values_[next];
  }
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance() {
  Advance(/*dist=*/1);
}

void ClassPermuterFactorialRadixDeleteTracking::Advancer::Advance(
    ValueSkip value_skip) {
  int value = current_[value_skip.value_index];
  while (!current_.empty() && current_[value_skip.value_index] == value) {
    int div = 1;
    for (int i = 1; i <= current_.size() - value_skip.value_index - 1; ++i) {
      div *= i;
    }
    int delta = div - (position_ % div);
    if (!active_set_.is_trivial()) {
      if (!active_set_.DiscardBlock(delta)) {
        delta += active_set_.ConsumeFalseBlock() + 1;
        CHECK(active_set_.ConsumeNext())
            << "ConsumeNext returned false after ConsumeFalseBlock";
      }
    }
    Advance(/*dist=*/delta);
  }
}

}  // namespace puzzle
