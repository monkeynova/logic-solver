#include "puzzle/active_set_bit_vector.h"

#include <algorithm>
#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace puzzle {

namespace {

std::vector<int> SortFlatHashSet(const absl::flat_hash_set<int>& unsorted) {
  std::vector<int> sorted;
  sorted.reserve(unsorted.size());
  std::copy(unsorted.begin(), unsorted.end(), std::back_inserter(sorted));
  std::sort(sorted.begin(), sorted.end());
  return sorted;
}

}  // namespace

// static
ActiveSetBitVector ActiveSetBitVectorBuilder::FromPositions(
    const absl::flat_hash_set<int>& positions, int max_position) {
  return FromPositions(SortFlatHashSet(positions), max_position);
}

// static
ActiveSetBitVector ActiveSetBitVectorBuilder::FromPositions(
    const std::initializer_list<int>& positions, int max_position) {
  return FromPositions(absl::flat_hash_set<int>(positions), max_position);
}

// static
ActiveSetBitVector ActiveSetBitVectorBuilder::FromPositions(
    const std::vector<int>& positions, int max_position) {
  ActiveSetBitVectorBuilder builder(max_position);
  for (auto p : positions) {
    if (p < 0) continue;
    if (p >= max_position) break;
    builder.AddBlockTo(false, p);
    builder.Add(true);
  }
  builder.AddBlockTo(false, max_position);
  return builder.DoneAdding();
}

ActiveSetBitVector ActiveSetBitVector::Intersection(
    const ActiveSetBitVector& other) const {
  if (other.is_trivial()) return *this;
  if (is_trivial()) return other;

  // TODO(@monkeynova): This should be a big bitwise-OR.
  ActiveSetBitVectorIterator this_iterator = GetIterator();
  ActiveSetBitVectorIterator other_iterator = other.GetIterator();

  VLOG(3) << "Intersect(" << DebugString() << ", " << other.DebugString()
          << ")";

  ActiveSetBitVectorBuilder intersection(std::max(total(), other.total()));
  while (this_iterator.more() && other_iterator.more()) {
    VLOG(3) << "Intersect NextBlock="
            << (this_iterator.value() ? "true" : "false") << "/\\"
            << (other_iterator.value() ? "true" : "false");
    bool next_run_value = false;
    int next_run_size;
    if (this_iterator.value() && other_iterator.value()) {
      // Both true, so true for run-length min.
      next_run_value = true;
      next_run_size =
          std::min(this_iterator.run_size(), other_iterator.run_size());
    } else if (this_iterator.value()) {
      // Single false (other since this is true), it dictactes length.
      next_run_size = other_iterator.run_size();
    } else if (other_iterator.value()) {
      // Single false (this since other is true), it dictactes length.
      next_run_size = this_iterator.run_size();
    } else {
      // Both false, so false for run-length max.
      next_run_size =
          std::max(this_iterator.run_size(), other_iterator.run_size());
    }
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
            << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    this_iterator.Advance(next_run_size);
    other_iterator.Advance(next_run_size);
  }
  while (this_iterator.more()) {
    bool next_run_value = this_iterator.value();
    int next_run_size = this_iterator.run_size();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
            << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    this_iterator.Advance(next_run_size);
  }
  while (other_iterator.more()) {
    bool next_run_value = other_iterator.value();
    int next_run_size = other_iterator.run_size();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
            << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    other_iterator.Advance(next_run_size);
  }

  const int intersection_total = std::max(total(), other.total());
  intersection.AddBlockTo(false, intersection_total);
  return intersection.DoneAdding();
}

std::string ActiveSetBitVector::DebugString() const {
  return absl::StrCat("{total:", total_, "; matches: {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSetBitVectorBuilder::Add(bool match) {
  DCHECK_LT(offset_, set_.total_);
  BitVector::SetBit(absl::MakeSpan(set_.matches_), offset_, match);
  ++offset_;
  if (match) {
    ++set_.matches_count_;
  }
}

void ActiveSetBitVectorBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;

  // TODO(@monkeynova): Better algorithm.
  for (int i = 0; i < size; ++i) {
    Add(match);
  }
}

ActiveSetBitVector ActiveSetBitVectorBuilder::DoneAdding() {
  CHECK_EQ(offset_, set_.total_);
  return std::move(set_);
}

std::string ActiveSetBitVectorIterator::DebugString() const {
  return absl::StrCat("offset: ", offset_, "; ", "total: ", total_, "; ",
                      "matches: {", absl::StrJoin(matches_, ","), "}");
}

int ActiveSetBitVectorIterator::run_size() const {
  // TODO(@monkeynova): Better algorithm.
  const bool current = BitVector::GetBit(matches_, offset_);
  int run_size = 1;
  for (; offset_ + run_size < total_ &&
	 BitVector::GetBit(matches_, offset_ + run_size) == current; ++run_size) {
    /* No-op*/
  }
  return run_size;
}

std::vector<int> ActiveSetBitVector::EnabledValues() const {
  ActiveSetBitVectorIterator it = GetIterator();
  std::vector<int> ret;
  while (it.more()) {
    int run_size = it.run_size();
    if (it.value()) {
      for (int i = 0; i < run_size; ++i) {
        ret.push_back(i + it.offset());
      }
    }
    it.Advance(run_size);
  }
  return ret;
}

std::string ActiveSetBitVector::DebugValues() const {
  return absl::StrCat("{", absl::StrJoin(EnabledValues(), ", "), "}");
}

}  // namespace puzzle
