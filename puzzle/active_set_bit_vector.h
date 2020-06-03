#ifndef PUZZLE_ACTIVE_SET_BIT_VECTOR_H
#define PUZZLE_ACTIVE_SET_BIT_VECTOR_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "glog/logging.h"

namespace puzzle {

class BitVector {
 public:
  // TODO(@monkeynova): Move to a 64bit word.
  using Word = uint64_t;
  static constexpr Word kBitsPerWord = sizeof(Word) * 8;
  static constexpr Word kAllBitsSet = 0xffffffffffffffffull;

  static void SetBit(absl::Span<Word> span, bool value, Word position) {
    DCHECK_LT(position, span.size() * kBitsPerWord);
    const Word bit_index = position % kBitsPerWord;
    span[position / kBitsPerWord] =
        (span[position / kBitsPerWord] & ~(1ull << bit_index)) |
        (static_cast<uint64_t>(value) << bit_index);
  }

  static void SetRange(absl::Span<Word> span, bool value, Word start,
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

  static bool GetBit(absl::Span<const Word> span, Word position) {
    DCHECK_LT(position, span.size() * kBitsPerWord);
    const Word bit_index = position % kBitsPerWord;
    return span[position / kBitsPerWord] & (1ull << bit_index);
  }

  static int GetRange(absl::Span<const Word> span, Word position, Word max) {
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
};

class ActiveSetBitVectorIterator {
 public:
  ActiveSetBitVectorIterator(absl::Span<const BitVector::Word> matches,
                             int total)
      : matches_(matches), total_(total) {}

  int offset() const { return offset_; }
  int total() const { return total_; }

  bool value() const { return BitVector::GetBit(matches_, offset_); }

  bool more() const { return offset() < total(); }

  int RunSize() const;

  void Advance(int n) { offset_ += n; }

  std::string DebugString() const;

 private:
  absl::Span<const BitVector::Word> matches_;
  int offset_ = 0;
  int total_ = 0;
};

// Forward declare for using ActiveSetBitVector::Builder.
class ActiveSetBitVectorBuilder;

class ActiveSetBitVector {
 public:
  using Iterator = ActiveSetBitVectorIterator;
  using Builder = ActiveSetBitVectorBuilder;

  static const ActiveSetBitVector& trivial() {
    static ActiveSetBitVector trivial = []() { return ActiveSetBitVector(); }();
    return trivial;
  }

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSetBitVector.
  ActiveSetBitVector(const ActiveSetBitVector& other) = default;
  ActiveSetBitVector& operator=(const ActiveSetBitVector& other) = default;
  ActiveSetBitVector(ActiveSetBitVector&& other) = default;
  ActiveSetBitVector& operator=(ActiveSetBitVector&& other) = default;

  // Returns the intersections of the two active sets (that is, returns an
  // ActiveSetBitVector which returns a true value for position if that value
  // position corresponds to true values in both 'this' and 'other'). If 'this'
  // and 'other' have different lengths, the intersections behaves as though the
  // shorter were padded with 'true' values to the longer.
  ActiveSetBitVector Intersection(const ActiveSetBitVector& other) const {
    ActiveSetBitVector ret = *this;
    ret.Intersect(other);
    return ret;
  }
  void Intersect(const ActiveSetBitVector& other);

  std::vector<int> EnabledValues() const;

  std::string DebugString() const;

  std::string DebugValues() const;

  bool is_trivial() const { return matches_count_ == total_; }
  int matches() const { return matches_count_; }
  int total() const { return total_; }
  double Selectivity() const {
    if (is_trivial()) return 1.0;
    return static_cast<double>(matches()) / total();
  }

  ActiveSetBitVectorIterator GetIterator() const {
    return ActiveSetBitVectorIterator(absl::MakeSpan(matches_), total_);
  }

 private:
  ActiveSetBitVector() = default;

  // ...
  // TODO(@monkeynova): Using a raw pointer could avoid 0-initialization costs.
  std::vector<BitVector::Word> matches_;

  // The total number of true values contained within this ActiveSetBitVector.
  int matches_count_ = 0;

  // The total number of boolean values contained within this
  // ActiveSetBitVector.
  int total_ = 0;

  friend class ActiveSetBitVectorBuilder;
};

class ActiveSetBitVectorBuilder {
 public:
  explicit ActiveSetBitVectorBuilder(int total) {
    set_.total_ = total;
    const int buf_size =
        (total + BitVector::kBitsPerWord - 1) / BitVector::kBitsPerWord;
    set_.matches_.resize(buf_size);
  }

  // Constructs an ActiveSetBitVector such that each value contained in
  // 'positions' returns 'true' and every other value in [0, 'max_position')
  // returns false.
  static ActiveSetBitVector FromPositions(
      const absl::flat_hash_set<int>& positions, int max_position);
  // Same as flat_hash_set form, except positions is required to be sorted.
  static ActiveSetBitVector FromPositions(const std::vector<int>& positions,
                                          int max_position);
  static ActiveSetBitVector FromPositions(
      const std::initializer_list<int>& positions, int max_position);

  // Adds a new boolean value to the current ActiveSetBitVector. Must not be
  // called after DoneAdding is called.
  void Add(bool match);

  // Adds `size` enties of `value`. Equivalent to:
  // for (int i = 0; i < size; ++i) Add(value);
  void AddBlock(bool value, int size);

  // Adds entites of `value` until `total()` is `position`. Note the fence-post
  // here. The value at `position` is left unset.
  void AddBlockTo(bool value, int position) {
    AddBlock(value, position - offset_);
  }

  // Returns the ActiveSetBitVector constructed by calls to Add and AddBlock. It
  // is undefined behavior to call more than once.
  ActiveSetBitVector DoneAdding();

  int total() { return set_.total(); }

 private:
  ActiveSetBitVector set_;

  // The total number of elements added to this builder.
  int offset_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_BIT_VECTOR_H
