#ifndef __PUZZLE_ACTIVE_SET_H
#define __PUZZLE_ACTIVE_SET_H

#include <vector>

namespace puzzle {

class ActiveSet {
 public:
  ActiveSet() = default;

  // Copy and move constructors preserve the add/consume phase of the
  // ActiveSet.
  ActiveSet(const ActiveSet& other) = default;
  ActiveSet& operator=(const ActiveSet& other) = default;
  ActiveSet(ActiveSet&& other) = default;
  ActiveSet& operator=(ActiveSet&& other) = default;

  // Adds a new boolean value to the current ActiveSet. Must not be called
  // after DoneAdding is called.
  void AddSkip(bool skip);

  // Called to indicate that the add phase is over and the consume phase
  // may not begin. Must be called before ConsumeNextSkip.
  void DoneAdding();
  
  // Returns whether or not to skip the current record and advances index
  // structures through skips_.
  // Must be called after DoneAdding is called.
  bool ConsumeNextSkip();

  bool is_trivial() const { return skips_.empty(); }
  int matches() const { return matches_; }
  int total() const { return total_; }
  double Selectivity() const {
    return static_cast<double>(matches()) / total();
  }
  
 private:
  // Thar be dragons here.
  // 'skips_' is a vector of ints representing runs of boolean conditions.
  // The first element corresponds to a run of "true" (i.e. should return)
  // permutations and each subsequent element negates the logic of the
  // previous run.
  // To start a run with "false", insert a 0 record at the first position.
  std::vector<int> skips_;
  bool building_ = true;

  // Indicates the current matching value. During the add phases indicates
  // the state of the current accumulating run. During the consume phase
  // indicates the state of the current consuming run.
  bool skip_match_ = true;

  // Indicates the length of the current accumlating run in the add phase.
  // Indicates the index of the current consuming run in 'skips_' during
  // the consume phase.
  int skips_position_ = 0;

  // The total number of true values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int matches_ = 0;

  // The total number of boolean values contained within this ActiveSet.
  // Immutable after DoneAdding is called.
  int total_ = 0;
};
  
}  // namespace puzzle

#endif  // __PUZZLE_ACTIVE_SET_H