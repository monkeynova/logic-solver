#ifndef PUZZLE_ACTIVE_SET_RUN_POSITION_H
#define PUZZLE_ACTIVE_SET_RUN_POSITION_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"

namespace puzzle {

class ActiveSetRunPositionIterator {
 public:
  ActiveSetRunPositionIterator(absl::Span<const int> matches, int total)
      : matches_(matches), total_(total) {
    // ActiveSetRunPosition may be constructed with an empty first record (it
    // uses this to indicate a false record to start), so skip that if present
    // and negate value.
    if (!matches_.empty() && matches_[0] == 0) {
      Advance(0);
    }
  }

  int offset() const { return offset_; }
  int total() const { return total_; }

  bool value() const { return !(match_position_ & 1); }

  bool more() const { return offset() < total(); }

  int RunSize() const {
    return match_position_ >= matches_.size()
               ? 0
               : matches_[match_position_] - offset_;
  }

  // Moves the iterator the next 'block_size' values.
  void Advance(int n);

  std::string DebugString() const;

 private:
  absl::Span<const int> matches_;
  int match_position_ = 0;
  int offset_ = 0;
  int total_ = 0;
};

// Forward declare for using ActiveSetRunPosition::Builder.
class ActiveSetRunPositionBuilder;

class ActiveSetRunPosition {
 public:
  using Iterator = ActiveSetRunPositionIterator;
  using Builder = ActiveSetRunPositionBuilder;

  static const ActiveSetRunPosition& trivial() {
    static ActiveSetRunPosition trivial = []() {
      return ActiveSetRunPosition();
    }();
    return trivial;
  }

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSetRunPosition.
  ActiveSetRunPosition(const ActiveSetRunPosition& other) = default;
  ActiveSetRunPosition& operator=(const ActiveSetRunPosition& other) = default;
  ActiveSetRunPosition(ActiveSetRunPosition&& other) = default;
  ActiveSetRunPosition& operator=(ActiveSetRunPosition&& other) = default;

  // Returns the intersections of the two active sets (that is, returns an
  // ActiveSetRunPosition which returns a true value for position if that value
  // position corresponds to true values in both 'this' and 'other'). If 'this'
  // and 'other' have different lengths, the intersections behaves as though the
  // shorter were padded with 'true' values to the longer.
  ActiveSetRunPosition Intersection(const ActiveSetRunPosition& other) const;
  void Intersect(const ActiveSetRunPosition& other) {
    if (other.is_trivial()) return;

    if (is_trivial())
      *this = other;
    else
      *this = Intersection(other);
  }

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

  ActiveSetRunPositionIterator GetIterator() const {
    return ActiveSetRunPositionIterator(absl::MakeSpan(matches_), total_);
  }

 private:
  ActiveSetRunPosition() = default;

  // Thar be dragons here.
  // 'matches_' is a vector of ints representing runs of boolean conditions.
  // The first element corresponds to a run of "true" (i.e. should return)
  // permutations and each subsequent element negates the logic of the
  // previous run.
  // To start a run with "false", insert a 0 record at the first position.
  std::vector<int> matches_;

  // The total number of true values contained within this ActiveSetRunPosition.
  int matches_count_ = 0;

  // The total number of boolean values contained within this
  // ActiveSetRunPosition.
  int total_ = 0;

  friend class ActiveSetRunPositionBuilder;
};

class ActiveSetRunPositionBuilder {
 public:
  explicit ActiveSetRunPositionBuilder(int total) { set_.total_ = total; }

  // Constructs an ActiveSetRunPosition such that each value contained in
  // 'positions' returns 'true' and every other value in [0, 'max_position')
  // returns false.
  static ActiveSetRunPosition FromPositions(
      const absl::flat_hash_set<int>& positions, int max_position);
  // Same as flat_hash_set form, except positions is required to be sorted.
  static ActiveSetRunPosition FromPositions(const std::vector<int>& positions,
                                            int max_position);
  static ActiveSetRunPosition FromPositions(
      const std::initializer_list<int>& positions, int max_position);

  // Adds a new boolean value to the current ActiveSetRunPosition. Must not be
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

  // Returns the ActiveSetRunPosition constructed by calls to Add and AddBlock.
  // It is undefined behavior to call more than once.
  ActiveSetRunPosition DoneAdding();

  int total() { return set_.total(); }

 private:
  ActiveSetRunPosition set_;

  // The boolean value of the current accumulating run.
  bool current_value_ = true;

  // The total number of elements added to this builder.
  int offset_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_RUN_POSITION_H
