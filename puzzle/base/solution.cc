#include "puzzle/base/solution.h"

namespace puzzle {

Entry Entry::invalid_(-1);

Solution& Solution::operator=(Solution&& other) {
  entry_descriptor_ = other.entry_descriptor_;
  entries_ = other.entries_;
  other.entries_ = nullptr;
  own_entries_ = other.own_entries_;
  permutation_position_ = other.permutation_position_;
  return *this;
}

Solution Solution::Clone() const {
  const std::vector<Entry>* new_entries =
      (entries_ == nullptr) ? nullptr : new std::vector<Entry>(*entries_);
  Solution ret(entry_descriptor_, new_entries);
  ret.own_entries_ = true;
  ret.permutation_position_ = permutation_position_;
  return ret;
}

bool Solution::operator==(const Solution& other) const {
  if (this == &other) {
    return true;
  }
  if (entries_ == nullptr || other.entries_ == nullptr) {
    return entries_ == other.entries_;
  }
  return *entries_ == *other.entries_;
}

}  // namespace puzzle
