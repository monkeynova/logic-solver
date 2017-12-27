#include "puzzle/active_set.h"

#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace puzzle {

std::string ActiveSet::DebugString() const {
  return absl::StrCat("{", (building_ ? "[building]" : "[built]"),
		      " ", (skip_match_ ? "match" : "skip"),
		      " ", skips_position_, " {", absl::StrJoin(skips_, ", "),
		      "}}");
}
  
void ActiveSet::AddSkip(bool skip) {
  CHECK(building_) << "AddSkip called after building";

  ++total_;
  if (skip) {
    ++matches_;
  }
  
  if (skip == skip_match_) {
    ++skips_position_;
  } else {
    skips_.push_back(skips_position_);
    skip_match_ = skip;
    skips_position_ = 1;
  }
}

void ActiveSet::DoneAdding() {
  building_ = false;
  if (skips_.empty()) {
    CHECK(skip_match_) << "skip_match shouldn't be false if skips is empty";
    // As a special case, if all entries are "true", we don't make skips_ so
    // the ActiveSet remains 'trivial'.
    skips_position_ = 0;
    return;
  }
  skips_.push_back(skips_position_);
  skip_match_ = true;
  skips_position_ = 0;
}

bool ActiveSet::ConsumeNextSkip() {
  CHECK(!building_) << "ConsumeNextSkip called while still building";

  if (skips_.empty()) return true;
  if (skips_position_ >= skips_.size()) return true;

  if (skips_[skips_position_] == 0) {
    skip_match_ = !skip_match_;
    ++skips_position_;
  }
  --skips_[skips_position_];
  return skip_match_;
}

int ActiveSet::ConsumeFalseBlock() {
  CHECK(!building_) << "ConsumeFalseBlock called while still building";

  if (skips_.empty()) return 0;
  if (skips_position_ >= skips_.size()) return 0;
  
  if (skips_[skips_position_] == 0) {
    skip_match_ = !skip_match_;
    ++skips_position_;
  }
  if (skip_match_) return 0;
  
  int ret = skips_[skips_position_];
  ++skips_position_;
  skip_match_ = true;
  return ret;
}
  
}
