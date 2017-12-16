#include "puzzle/active_set.h"

#include <iostream>

namespace puzzle {

void ActiveSet::AddSkip(bool skip) {
  if (!building_) {
    std::cerr << "AddSkip called after building" << std::endl;
    return;
  }

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
  skips_.push_back(skips_position_);
  skip_match_ = true;
  skips_position_ = 0;
}

bool ActiveSet::ConsumeNextSkip() {
  if (building_) {
    std::cerr << "ConsumeNextSkip called while still building" << std::endl;
    return true;
  }
  if (skips_.empty()) return true;

  if (skips_[skips_position_] == 0) {
    skip_match_ = !skip_match_;
    ++skips_position_;
  }
  --skips_[skips_position_];
  return skip_match_;
}

}
