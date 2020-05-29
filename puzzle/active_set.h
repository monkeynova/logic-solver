#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"

namespace puzzle {

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

  // Returns whether or not to skip the current record and advances index
  // structures through matches_.
  // Must be called after DoneAdding is called.
  bool ConsumeNext();

  // Consumes the next run of false records and returns the number of records
  // consumed.
  // Must be called after DoneAdding is called.
  int ConsumeFalseBlock();

  // Consumes the next 'block_size' values and returns the last value from the
  // block.
  bool DiscardBlock(int block_size);

  bool is_trivial() const { return matches_.empty(); }
  int matches() const { return matches_count_; }
  int total() const { return total_; }
  int offset() const { return offset_; }
  double Selectivity() const {
    if (is_trivial()) return 1.0;
    return static_cast<double>(matches()) / total();
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

  // Indicates the current matching value. During the add phases indicates
  // the state of the current accumulating run. During the consume phase
  // indicates the state of the current consuming run.
  bool current_value_ = true;

  // Indicates the length of the current accumlating run in the add phase.
  // Indicates the index of the current consuming run in 'matches_' during
  // the consume phase.
  int matches_position_ = 0;

  // The total number of true values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int matches_count_ = 0;

  // The total number of boolean values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int total_ = 0;

  // The number of results consumed so far from this ActiveSet.
  int offset_ = 0;

  friend class ActiveSetBuilder;
  friend class ActiveSetIterator;
};

class ActiveSetIterator {
 public:
  explicit ActiveSetIterator(const ActiveSet& active_set)
    : matches_(absl::MakeSpan(active_set.matches_)
	           .subspan(active_set.matches_position_)),
      value_(active_set.current_value_) {}

  bool value() const { return value_; }

  bool more() const {
    return match_position_ < static_cast<int>(matches_.size());
  }

  int run_size() const { return matches_[match_position_] - run_position_; }

  void Advance(int n) {
    run_position_ += n;
    while (more() && run_position_ >= matches_[match_position_]) {
      run_position_ -= matches_[match_position_];
      ++match_position_;
      value_ = !value_;
    }
  }

 private:
  absl::Span<const int> matches_;
  bool value_;
  int match_position_ = 0;
  int run_position_ = 0;
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

  // Adds 'size' enties of 'value'. Equivalent to:
  // for (int i = 0; i < size; ++i) Add(value);
  void AddBlock(bool value, int size);

  // Returns the ActiveSet constructed by calls to Add and AddBlock. It is
  // undefined behavior to call more than once.
  ActiveSet DoneAdding();

  int total() { return set_.total(); }

 private:
  ActiveSet set_;
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
