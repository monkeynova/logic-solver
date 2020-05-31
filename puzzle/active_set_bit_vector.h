#ifndef PUZZLE_ACTIVE_SET_BIT_VECTOR_H
#define PUZZLE_ACTIVE_SET_BIT_VECTOR_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"

namespace puzzle {

class ActiveSetBitVectorIterator {
 public:
  ActiveSetBitVectorIterator(absl::Span<const int> matches, bool value,
                             int total)
      : matches_(matches), value_(value), total_(total) {}

  int offset() const { return offset_; }
  int total() const { return total_; }

  bool value() const { return value_; }

  bool more() const { return offset() < total(); }

  int run_size() const {
    if (match_position_ >= matches_.size()) return total() - offset();
    return matches_[match_position_] - run_position_;
  }

  // Moves the iterator the next 'block_size' values.
  void Advance(int n);

  std::string DebugString() const;

 private:
  absl::Span<const int> matches_;
  bool value_;
  int match_position_ = 0;
  int run_position_ = 0;
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
  ActiveSetBitVector Intersection(const ActiveSetBitVector& other) const;
  void Intersect(const ActiveSetBitVector& other) {
    if (other.is_trivial()) return;

    if (is_trivial())
      *this = other;
    else
      *this = Intersection(other);
  }

  std::vector<int> EnabledValues() const;

  std::string DebugString() const;

  std::string DebugValues() const;

  bool is_trivial() const { return matches_.empty(); }
  int matches() const { return matches_count_; }
  int total() const { return total_; }
  double Selectivity() const {
    if (is_trivial()) return 1.0;
    return static_cast<double>(matches()) / total();
  }

  ActiveSetBitVectorIterator GetIterator() const {
    // ActiveSetBitVector may be constructed with an empty first record (it uses
    // this to indicate a false record to start), so skip that if present and
    // negate value.
    if (!matches_.empty() && matches_[0] == 0) {
      return ActiveSetBitVectorIterator(absl::MakeSpan(matches_).subspan(1),
                                        false, total_);
    }
    return ActiveSetBitVectorIterator(absl::MakeSpan(matches_), true, total_);
  }

 private:
  ActiveSetBitVector() = default;

  // Thar be dragons here.
  // 'matches_' is a vector of ints representing runs of boolean conditions.
  // The first element corresponds to a run of "true" (i.e. should return)
  // permutations and each subsequent element negates the logic of the
  // previous run.
  // To start a run with "false", insert a 0 record at the first position.
  std::vector<int> matches_;

  // The total number of true values contained within this ActiveSetBitVector.
  int matches_count_ = 0;

  // The total number of boolean values contained within this
  // ActiveSetBitVector.
  int total_ = 0;

  friend class ActiveSetBitVectorBuilder;
};

class ActiveSetBitVectorBuilder {
 public:
  explicit ActiveSetBitVectorBuilder(int total) { set_.total_ = total; }

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

  // The boolean value of the current accumulating run.
  bool current_value_ = true;

  // The length of the current accumlating run.
  int run_size_ = 0;

  // The total number of elements added to this builder.
  int offset_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_BIT_VECTOR_H
