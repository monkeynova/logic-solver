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
    AddFalseBlock(p - last_p - 1);
    Add(true);
    last_p = p;
  }
  if (last_p < max_position - 1) {
    AddFalseBlock(max_position - last_p - 1);
  }
  DoneAdding();
}

ActiveSet ActiveSet::Intersect(const ActiveSet& other) const {
  ActiveSet this_copy = *this;
  ActiveSet other_copy = other;

  ActiveSet ret;
  int new_total = std::max(this_copy.total(), other_copy.total());
  for (int i = 0; i < new_total; ++i) {
    bool this_next = this_copy.ConsumeNext();
    bool other_next = other_copy.ConsumeNext();
    ret.Add(this_next && other_next);
  }
  ret.DoneAdding();
  return ret;
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

void ActiveSet::AddFalseBlock(int size) {
  for (int i = 0; i < size; ++i) Add(/*match=*/false);
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
