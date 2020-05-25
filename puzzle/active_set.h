#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"

namespace puzzle {

class ActiveSet {
 public:
  static const ActiveSet& trivial() {
    static ActiveSet trivial = []() {
      ActiveSet ret;
      ret.DoneAdding();
      return ret;
    }();
    return trivial;
  }

  ActiveSet() = default;

  // Constructs an ActiveSet such that each value contained in 'positions'
  // returns 'true' and every other value in [0, 'max_position') returns false.
  ActiveSet(const absl::flat_hash_set<int>& positions, int max_position);
  // Same as flat_hash_set form, except positions is required to be sorted.
  ActiveSet(const std::vector<int>& positions, int max_position);
  ActiveSet(const std::initializer_list<int>& positions, int max_position)
      : ActiveSet(absl::flat_hash_set<int>(positions), max_position) {}

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

  // Adds a new boolean value to the current ActiveSet. Must not be called
  // after DoneAdding is called.
  void Add(bool match);

  // Adds 'size' enties of 'value'. Equivalent to:
  // for (int i = 0; i < size; ++i) Add(value);
  void AddBlock(bool value, int size);

  // Called to indicate that the add phase is over and the consume phase
  // may not begin. Must be called before ConsumeNext.
  void DoneAdding();

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
  double Selectivity() const {
    if (is_trivial()) return 1.0;
    return static_cast<double>(matches()) / total();
  }

 private:
  // Thar be dragons here.
  // 'matches_' is a vector of ints representing runs of boolean conditions.
  // The first element corresponds to a run of "true" (i.e. should return)
  // permutations and each subsequent element negates the logic of the
  // previous run.
  // To start a run with "false", insert a 0 record at the first position.
  std::vector<int> matches_;

  // Internal state to verify that Add*, DoneAdding, Consume* are called in
  // that order. True means Add* calls are allowed while Consume* calls are
  // not.
  bool building_ = true;

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
};

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
