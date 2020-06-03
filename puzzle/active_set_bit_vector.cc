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
void BitVector::SetRange(absl::Span<Word> span, bool value, Word start,
			 Word end) {
  DCHECK_LT(start, span.size() * kBitsPerWord);
  DCHECK_LT(end, span.size() * kBitsPerWord);
  Word write_word = start / kBitsPerWord;
  Word end_word = end / kBitsPerWord;
  Word mask = kAllBitsSet << (start % kBitsPerWord);
  for (; write_word != end_word; ++write_word) {
    if (value) {
      span[write_word] |= mask;
    } else {
      span[write_word] &= ~mask;
    }
    mask = kAllBitsSet;
  }
  mask &= kAllBitsSet >> (kBitsPerWord - (end % kBitsPerWord));
  if (value) {
    span[write_word] |= mask;
  } else {
    span[write_word] &= ~mask;
  }
}

// static
int BitVector::GetRange(absl::Span<const Word> span, Word position, Word max) {
  DCHECK_LT(position, span.size() * kBitsPerWord);
  DCHECK_LT(max, span.size() * kBitsPerWord);
  Word read_word = position / kBitsPerWord;
  Word end_word = max / kBitsPerWord;
  const Word start_bit = position % kBitsPerWord;
  const bool is_run_set = span[read_word] & (1ull << start_bit);
  Word mask = kAllBitsSet << start_bit;
  Word run_size = -start_bit;
  for (; read_word != end_word; ++read_word) {
    Word read_bits = mask & (is_run_set ? ~span[read_word] : span[read_word]);
    if (read_bits) {
      static_assert(sizeof(Word) == 8,
		    "ffs implementation calls uint64_t override");
      run_size += __builtin_ffsll(read_bits) - 1;
      return run_size;
    }
    run_size += kBitsPerWord;
    mask = kAllBitsSet;
  }
  mask &= ~(kAllBitsSet << (max % kBitsPerWord));
  Word read_bits = mask & (is_run_set ? ~span[read_word] : span[read_word]);
  if (read_bits) {
    run_size += __builtin_ffsll(read_bits) - 1;
  } else {
    run_size += max % kBitsPerWord;
  }
  return run_size;
}

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
