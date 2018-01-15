#ifndef __PUZZLE_ACTIVE_SET_H
#define __PUZZLE_ACTIVE_SET_H

#include <set>
#include <string>
#include <vector>

namespace puzzle {

class ActiveSet {
 public:
  ActiveSet() = default;

  // Constructs an ActiveSet such that each value contained in 'positions'
  // returns 'true' and every other value in [0, 'max_position') returns false.
  ActiveSet(const std::set<int>& positions, int max_position);

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSet.
  ActiveSet(const ActiveSet& other) = default;
  ActiveSet& operator=(const ActiveSet& other) = default;
  ActiveSet(ActiveSet&& other) = default;
  ActiveSet& operator=(ActiveSet&& other) = default;

  void Intersect(const ActiveSet& other);
  ActiveSet Intersection(const ActiveSet& other) const {
    ActiveSet ret = *this;
    ret.Intersect(other);
    return ret;
  }

  std::string DebugString() const;

  // Adds a new boolean value to the current ActiveSet. Must not be called
  // after DoneAdding is called.
  void Add(bool match);

  // Adds 'size' false enties. Equivalent to:
  // for (int i = 0; i < size; ++i) Add(false);
  void AddFalseBlock(int size);

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

#endif  // __PUZZLE_ACTIVE_SET_H
