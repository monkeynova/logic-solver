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

void ActiveSet::Intersect(const ActiveSet& other) {
  ActiveSet this_copy = *this;
  ActiveSet other_copy = other;

  *this = ActiveSet();
  int new_total = std::max(this_copy.total(), other_copy.total());
  for (int i = 0; i < new_total; ++i) {
    bool this_next = this_copy.ConsumeNext();
    bool other_next = other_copy.ConsumeNext();
    Add(this_next && other_next);
  }
  DoneAdding();
}

#if 0
void ActiveSet::Intersect(const ActiveSet& other) {
  CHECK(!building_) << "Intersect called during building";
  CHECK(!other.building_) << "Intersect called with an unbuilt ActiveSet";

  bool this_value = true;
  int this_match_position = 0;
  int this_run_position = 0;
  
  bool other_value = true;
  int other_match_position = 0;
  int other_run_position = 0;

  ActiveSet intersection;
  while (this_match_position < matches_.size() ||
	 other_match_position < other.matches_.size()) {
    bool next_run_value = this_value && other_value;
    int next_run_size = std::numeric_limits<int>::max();
    if (this_match_position < matches_.size()) {
      next_run_size = std::min(
          next_run_size, matches_[this_match_position] - this_run_position);
    }
    if (other_match_position < other.matches_.size()) {
      next_run_size = std::min(
          next_run_size,
	  other.matches_[other_match_position] - other_run_position);
    }
    // Advance this by 'next_run_size'.
    if (this_match_position < matches_.size()) {
      this_run_position += next_run_size;
      if (this_run_position >= matches_[this_match_position]) {
	++this_match_position;
	this_run_position = 0;
	this_value = !this_value || this_match_position >= matches_.size();
      }
    }
    // Advance other by 'next_run_size'.
    if (other_match_position < other.matches_.size()) {
      other_run_position += next_run_size;
      if (other_run_position >= other.matches_[other_match_position]) {
	++other_match_position;
	other_run_position = 0;
	this_value =
	    !other_value || other_match_position >= other.matches_.size();
      }
    }
    // Store 'next_run_size' values of 'next_run_value'.
    intersection.AddBlock(next_run_value, next_run_size);
  }
  // Mark done and update self.
  intersection.DoneAdding();
  *this = std::move(intersection);
}
#endif

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
