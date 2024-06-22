#include "puzzle/active_set/bit_vector.h"

#include <algorithm>
#include <iostream>
#ifdef _MSC_VER
#include <intrin.h>
#include <nmmintrin.h>
#endif

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "vlog.h"

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

void BitVector::SetRange(bool value, Word start, Word end) {
  DCHECK_LT(start, num_bits_);
  DCHECK_LE(end, num_bits_);
  Word write_word = start / kBitsPerWord;
  Word end_word = end / kBitsPerWord;
  Word mask = kAllBitsSet << (start % kBitsPerWord);
  for (; write_word != end_word; ++write_word) {
    if (value) {
      buf_[write_word] |= mask;
    } else {
      buf_[write_word] &= ~mask;
    }
    mask = kAllBitsSet;
  }
  mask &= kAllBitsSet >> (kBitsPerWord - (end % kBitsPerWord));
  if (value) {
    buf_[write_word] |= mask;
  } else {
    buf_[write_word] &= ~mask;
  }
}

int BitVector::GetRange(Word position) const {
  DCHECK_LT(position, num_bits_);
  Word read_word = position / kBitsPerWord;
  Word end_word = num_bits_ / kBitsPerWord;
  const Word start_bit = position % kBitsPerWord;
  const bool is_run_set = buf_[read_word] & (1ull << start_bit);
  Word mask = kAllBitsSet << start_bit;
  Word run_size = -start_bit;
  for (; read_word != end_word; ++read_word) {
    Word read_bits = mask & (is_run_set ? ~buf_[read_word] : buf_[read_word]);
    if (read_bits) {
      static_assert(sizeof(Word) == 8,
                    "ffs implementation calls uint64_t override");
#ifdef _MSC_VER
      unsigned long index;
      CHECK(_BitScanForward64(&index, read_bits));
      run_size += index;
#else
      run_size += __builtin_ffsll(read_bits) - 1;
#endif
      return run_size;
    }
    run_size += kBitsPerWord;
    mask = kAllBitsSet;
  }
  mask &= ~(kAllBitsSet << (num_bits_ % kBitsPerWord));
  Word read_bits = mask & (is_run_set ? ~buf_[read_word] : buf_[read_word]);
  if (read_bits) {
#ifdef _MSC_VER
    unsigned long index;
    CHECK(_BitScanForward64(&index, read_bits));
    run_size += index;
#else
    run_size += __builtin_ffsll(read_bits) - 1;
#endif
  } else {
    run_size += num_bits_ % kBitsPerWord;
  }
  return run_size;
}

std::string BitVector::DebugString() const {
  return absl::StrCat("num_bits: ", num_bits_, " {",
                      absl::StrJoin(absl::MakeSpan(buf_, num_words()), ",",
                                    [](std::string* out, const Word& v) {
                                      absl::StrAppend(out, absl::Hex(v));
                                    }),
                      "}");
}

int BitVector::Intersect(const BitVector* other) {
  CHECK_EQ(num_bits_, other->num_bits_) << "Mismatched lengths not supported";
  int ret = 0;
  for (int i = 0; i < num_words(); ++i) {
    buf_[i] &= other->buf_[i];
#ifdef _MSC_VER
    ret += _mm_popcnt_u64(buf_[i]);
#else
    ret += __builtin_popcountll(buf_[i]);
#endif
  }
  if (num_words() > 0 && (num_bits() % kBitsPerWord) != 0) {
    // Don't count the bits in the last word past the end.
#ifdef _MSC_VER
    ret -= _mm_popcnt_u64(buf_[num_words() - 1] &
                          (kAllBitsSet << (num_bits() % kBitsPerWord)));
#else
    ret -= __builtin_popcountll(buf_[num_words() - 1] &
                                (kAllBitsSet << (num_bits() % kBitsPerWord)));
#endif
  }
  return ret;
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
  matches_count_ = matches_->Intersect(other.matches_.get());
}

std::string ActiveSetBitVector::DebugString() const {
  return matches_->DebugString();
}

void ActiveSetBitVectorBuilder::Add(bool match) {
  DCHECK_LT(offset_, set_.total());
  set_.matches_->SetBit(match, offset_);
  ++offset_;
  if (match) {
    ++set_.matches_count_;
  }
}

void ActiveSetBitVectorBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;
  DCHECK_LT(offset_, set_.total());

  set_.matches_->SetRange(match, offset_, offset_ + size);
  offset_ += size;
  if (match) {
    set_.matches_count_ += size;
  }
}

ActiveSetBitVector ActiveSetBitVectorBuilder::DoneAdding() {
  CHECK_EQ(offset_, set_.total());
  return std::move(set_);
}

std::string ActiveSetBitVectorIterator::DebugString() const {
  return absl::StrCat("offset: ", offset_, "; ",
                      "matches: ", matches_->DebugString());
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
