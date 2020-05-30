#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"

namespace puzzle {

class ActiveSetIterator {
 public:
  ActiveSetIterator(absl::Span<const int> matches, bool value, int total)
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

class ActiveSet {
 public:
  static const ActiveSet& trivial() {
    static ActiveSet trivial = []() { return ActiveSet(); }();
    return trivial;
  }

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSet.
  ActiveSet(const ActiveSet& other) = default;
  ActiveSet& operator=(const ActiveSet& other) = default;
  ActiveSet(ActiveSet&& other) = default;
  ActiveSet& operator=(ActiveSet&& other) = default;

  // Returns the intersections of the two active sets (that is, returns an
  // ActiveSet which returns a true value for position if that value position
  // corresponds to true values in both 'this' and 'other').
  // If 'this' and 'other' have different lengths, the intersections behaves as
  // though the shorter were padded with 'true' values to the longer.
  ActiveSet Intersection(const ActiveSet& other) const;
  void Intersect(const ActiveSet& other) {
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

  ActiveSetIterator Iterator() const {
    // ActiveSet may be constructed with an empty first record (it uses this
    // to indicate a false record to start), so skip that if present and
    // negate value.
    if (!matches_.empty() && matches_[0] == 0) {
      return ActiveSetIterator(absl::MakeSpan(matches_).subspan(1), false,
                               total_);
    }
    return ActiveSetIterator(absl::MakeSpan(matches_), true, total_);
  }

 private:
  ActiveSet() = default;

  // Thar be dragons here.
  // 'matches_' is a vector of ints representing runs of boolean conditions.
  // The first element corresponds to a run of "true" (i.e. should return)
  // permutations and each subsequent element negates the logic of the
  // previous run.
  // To start a run with "false", insert a 0 record at the first position.
  std::vector<int> matches_;

  // The total number of true values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int matches_count_ = 0;

  // The total number of boolean values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int total_ = 0;

  friend class ActiveSetBuilder;
};

class ActiveSetBuilder {
 public:
  ActiveSetBuilder() = default;

  // Constructs an ActiveSet such that each value contained in 'positions'
  // returns 'true' and every other value in [0, 'max_position') returns false.
  static ActiveSet FromPositions(const absl::flat_hash_set<int>& positions,
                                 int max_position);
  // Same as flat_hash_set form, except positions is required to be sorted.
  static ActiveSet FromPositions(const std::vector<int>& positions,
                                 int max_position);
  static ActiveSet FromPositions(const std::initializer_list<int>& positions,
                                 int max_position);

  // Adds a new boolean value to the current ActiveSet. Must not be called
  // after DoneAdding is called.
  void Add(bool match);

  // Adds `size` enties of `value`. Equivalent to:
  // for (int i = 0; i < size; ++i) Add(value);
  void AddBlock(bool value, int size);

  // Adds entites of `value` until `total()` is `position`. Note the fence-post
  // here. The value at `position` is left unset.
  void AddBlockTo(bool value, int position) {
    AddBlock(value, position - total());
  }

  // Returns the ActiveSet constructed by calls to Add and AddBlock. It is
  // undefined behavior to call more than once.
  ActiveSet DoneAdding();

  int total() { return set_.total(); }

 private:
  ActiveSet set_;

  // Indicates the state of the current accumulating run.
  bool current_value_ = true;

  // Indicates the length of the current accumlating run.
  int matches_position_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
