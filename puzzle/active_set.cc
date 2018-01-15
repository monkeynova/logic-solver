#include "puzzle/active_set.h"

#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace puzzle {

ActiveSet::ActiveSet(const std::set<int>& positions, int max_position) {
  int last_p = -1;
  for (auto p : positions) {
    if (p < 0) continue;
    if (p >= max_position) break;
    AddBlock(false, p - last_p - 1);
    Add(true);
    last_p = p;
  }
  if (last_p < max_position - 1) {
    AddBlock(false, max_position - last_p - 1);
  }
  DoneAdding();
}

struct ActiveSetIterator {
  explicit ActiveSetIterator(const std::vector<int>& matches)
    : matches_(matches) {}

  bool value() const { return value_; }
  
  bool more() const { return match_position_ < matches_.size(); }
  
  int run_size() const { return matches_[match_position_] - run_position_; }

  void Advance(int n) {
    run_position_ += n;
    while (run_position_ >= matches_[match_position_]) {
      run_position_ -= matches_[match_position_];
      ++match_position_;
      value_ = !value_;
    }
  }

 private:
  const std::vector<int>& matches_;
  bool value_ = true;
  int match_position_ = 0;
  int run_position_ = 0;
};
  
void ActiveSet::Intersect(const ActiveSet& other) {
  CHECK(!building_) << "Intersect called during building";
  CHECK(!other.building_) << "Intersect called with an unbuilt ActiveSet";

  ActiveSetIterator this_iterator(matches_);
  ActiveSetIterator other_iterator(other.matches_);

  VLOG(3) << "Intersect(" << DebugString() << ", " << other.DebugString()
	  << ")";
  
  ActiveSet intersection;
  while (this_iterator.more() && other_iterator.more()) {
    VLOG(3) << "Intersect NextBlock="
	    << (this_iterator.value() ? "true" : "false")
	    << "/\\" << (other_iterator.value() ? "true" : "false");
    bool next_run_value = this_iterator.value() && other_iterator.value();
    int next_run_size = std::min(this_iterator.run_size(),
				 other_iterator.run_size());
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
	    << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    this_iterator.Advance(next_run_size);
    other_iterator.Advance(next_run_size);
  }
  while (this_iterator.more()) {
    bool next_run_value = this_iterator.value();
    int next_run_size = this_iterator.run_size();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
	    << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    this_iterator.Advance(next_run_size);
  }
  while (other_iterator.more()) {
    bool next_run_value = other_iterator.value();
    int next_run_size = other_iterator.run_size();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
	    << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    other_iterator.Advance(next_run_size);
  }

  intersection.total_ = std::max(total_, other.total_);
  // Mark done and update self.
  intersection.DoneAdding();
  VLOG(3) << "Intersect == " << intersection.DebugString();
  *this = std::move(intersection);
}

std::string ActiveSet::DebugString() const {
  return absl::StrCat("{", (building_ ? "[building]" : "[built]"),
                      " ", (current_value_ ? "match" : "skip"),
                      " ", matches_position_, " {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSet::Add(bool match) {
  CHECK(building_) << "Add called after building";

  ++total_;
  if (match) {
    ++matches_count_;
  }

  if (match == current_value_) {
    ++matches_position_;
  } else {
    matches_.push_back(matches_position_);
    current_value_ = match;
    matches_position_ = 1;
  }
}

void ActiveSet::AddBlock(bool match, int size) {
  CHECK(building_) << "Add called after building";
  if (size == 0) return;
  
  total_ += size;
  if (match) {
    matches_count_ += size;
  }

  if (match == current_value_) {
    matches_position_ += size;
  } else {
    matches_.push_back(matches_position_);
    current_value_ = match;
    matches_position_ = size;
  }
}

void ActiveSet::DoneAdding() {
  building_ = false;
  if (matches_.empty()) {
    CHECK(current_value_) << "skip_match shouldn't be false if skips is empty";
    // As a special case, if all entries are "true", we don't make matches_ so
    // the ActiveSet remains 'trivial'.
    matches_position_ = 0;
    return;
  }
  matches_.push_back(matches_position_);
  current_value_ = true;
  matches_position_ = 0;
}

bool ActiveSet::ConsumeNext() {
  CHECK(!building_) << "ConsumeNext called while still building";

  if (matches_.empty()) return true;
  if (matches_position_ >= matches_.size()) return true;

  if (matches_[matches_position_] == 0) {
    current_value_ = !current_value_;
    ++matches_position_;
    if (matches_position_ >= matches_.size()) return true;
  }
  --matches_[matches_position_];
  return current_value_;
}

int ActiveSet::ConsumeFalseBlock() {
  CHECK(!building_) << "ConsumeFalseBlock called while still building";

  if (matches_.empty()) return 0;
  if (matches_position_ >= matches_.size()) return 0;

  if (matches_[matches_position_] == 0) {
    current_value_ = !current_value_;
    ++matches_position_;
    if (matches_position_ >= matches_.size()) return 0;
  }
  if (current_value_) return 0;

  int ret = matches_[matches_position_];
  ++matches_position_;
  current_value_ = true;
  return ret;
}

}
