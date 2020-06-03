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

void ActiveSetBitVector::Intersect(const ActiveSetBitVector& other) {
  if (other.is_trivial()) return;
  if (is_trivial()) {
    *this = other;
    return;
  }
  if (total_ == other.total_) {
    for (int i = 0; i < matches_.size(); ++i) {
      matches_[i] &= other.matches_[i];
    }
    return;
  }
  CHECK(false) << "Mismatched lengths not supported";
}

std::string ActiveSetBitVector::DebugString() const {
  return absl::StrCat("{total:", total_, "; matches: {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSetBitVectorBuilder::Add(bool match) {
  DCHECK_LT(offset_, set_.total_);
  BitVector::SetBit(absl::MakeSpan(set_.matches_), match, offset_);
  ++offset_;
  if (match) {
    ++set_.matches_count_;
  }
}

void ActiveSetBitVectorBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;
  DCHECK_LT(offset_, set_.total_);

  BitVector::SetRange(absl::MakeSpan(set_.matches_), match, offset_,
                      offset_ + size);
  offset_ += size;
  if (match) {
    set_.matches_count_ += size;
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

int ActiveSetBitVectorIterator::RunSize() const {
  return BitVector::GetRange(matches_, offset_, total_);
}

std::vector<int> ActiveSetBitVector::EnabledValues() const {
  ActiveSetBitVectorIterator it = GetIterator();
  std::vector<int> ret;
  while (it.more()) {
    int run_size = it.RunSize();
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
