#ifndef PUZZLE_ACTIVE_SET_BIT_VECTOR_H
#define PUZZLE_ACTIVE_SET_BIT_VECTOR_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "glog/logging.h"

namespace puzzle {

class BitVector {
 public:
  struct BitVectorDeleter {
    void operator()(BitVector* bv) { free(bv); }
  };

  using UniquePtr = std::unique_ptr<BitVector, BitVectorDeleter>;
  using Word = uint64_t;
  static constexpr Word kBitsPerWord = sizeof(Word) * 8;
  static constexpr Word kAllBitsSet = 0xffffffffffffffffull;

  static UniquePtr Make(Word num_bits) {
    int num_words =
        (num_bits + BitVector::kBitsPerWord - 1) / BitVector::kBitsPerWord;
    BitVector* ret = static_cast<BitVector*>(
        malloc(sizeof(BitVector) + num_words * sizeof(Word)));
    ret->num_bits_ = num_bits;
    ret->num_words_ = num_words;
    return UniquePtr(ret);
  }

  UniquePtr Copy() {
    UniquePtr ret = Make(num_bits_);
    memcpy(ret->buf_, buf_, sizeof(Word) * num_words());
    return ret;
  }

  int num_bits() const { return num_bits_; }
  int num_words() const { return num_words_; }

  void SetBit(bool value, Word position) {
    DCHECK_LT(position, num_bits_);
    const Word bit_index = position % kBitsPerWord;
    buf_[position / kBitsPerWord] =
        (buf_[position / kBitsPerWord] & ~(1ull << bit_index)) |
        (static_cast<uint64_t>(value) << bit_index);
  }

  void SetRange(bool value, Word start, Word end);

  bool GetBit(Word position) const {
    DCHECK_LT(position, num_bits_);
    const Word bit_index = position % kBitsPerWord;
    return buf_[position / kBitsPerWord] & (1ull << bit_index);
  }

  int GetRange(Word position) const;

  // Makes `this` contain only bits also stored in `other`. Returns the number
  // of bits set in `this` after the intersection is performed.
  int Intersect(const BitVector* other);

  std::string DebugString() const;

 private:
  Word num_bits_;
  Word num_words_;
  Word buf_[];
};

class ActiveSetBitVectorIterator {
 public:
  explicit ActiveSetBitVectorIterator(const BitVector* matches)
      : matches_(matches) {}

  int offset() const { return offset_; }
  int total() const { return matches_->num_bits(); }

  bool value() const { return matches_->GetBit(offset_); }

  bool more() const { return offset() < total(); }

  int RunSize() const { return matches_->GetRange(offset_); }

  void Advance(int n) { offset_ += n; }

  std::string DebugString() const;

 private:
  const BitVector* matches_;
  int offset_ = 0;
};

// Forward declare for using ActiveSetBitVector::Builder.
class ActiveSetBitVectorBuilder;

class ActiveSetBitVector {
 public:
  using Iterator = ActiveSetBitVectorIterator;
  using Builder = ActiveSetBitVectorBuilder;

  static const ActiveSetBitVector& trivial() {
    static ActiveSetBitVector trivial = []() {
      return ActiveSetBitVector(/*total=*/0);
    }();
    return trivial;
  }

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSetBitVector.
  ActiveSetBitVector(const ActiveSetBitVector& other) {
    matches_count_ = other.matches_count_;
    matches_ = other.matches_->Copy();
  }
  ActiveSetBitVector& operator=(const ActiveSetBitVector& other) {
    matches_count_ = other.matches_count_;
    matches_ = other.matches_->Copy();
    return *this;
  }
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

  bool is_trivial() const { return matches_count_ == total(); }
  int matches() const { return matches_count_; }
  int total() const { return matches_->num_bits(); }
  double Selectivity() const {
    if (is_trivial()) return 1.0;
    return static_cast<double>(matches()) / total();
  }

  ActiveSetBitVectorIterator GetIterator() const {
    return ActiveSetBitVectorIterator(matches_.get());
  }

 private:
  explicit ActiveSetBitVector(int total) : matches_(BitVector::Make(total)) {}

  // Bit vector representation of the values in this set.
  BitVector::UniquePtr matches_;

  // The total number of true values contained within this ActiveSetBitVector.
  int matches_count_ = 0;

  friend class ActiveSetBitVectorBuilder;
};

class ActiveSetBitVectorBuilder {
 public:
  explicit ActiveSetBitVectorBuilder(int total) : set_(total) {}

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
