#include "puzzle/solution.h"

#include <sstream>

namespace puzzle {

std::string Entry::DebugString() const {
  std::stringstream ret;
  if (entry_descriptor_ != nullptr) {
    ret << entry_descriptor_->Id(id_);
  } else {
    ret << id_;
  }
  ret << ":";
  for (unsigned int i = 0; i < classes_.size(); ++i) {
    if (entry_descriptor_) {
      ret << " " << entry_descriptor_->Class(i) << "="
          << entry_descriptor_->Name(i, classes_[i]);
    } else {
      ret << " " << classes_[i];
    }
  }
  return ret.str();
}
  
Solution& Solution::operator=(Solution&& other) {
  entry_descriptor_ = other.entry_descriptor_;
  entries_ = other.entries_;
  other.entries_ = nullptr;
  own_entries_ = other.own_entries_;
  permutation_position_ = other.permutation_position_;
  permutation_count_ = other.permutation_count_;
  return *this;
}

Solution Solution::Clone() const {
  const std::vector<Entry>* new_entries =
    (entries_ == nullptr) ? nullptr : new std::vector<Entry>(*entries_);
  Solution ret(entry_descriptor_, new_entries);
  ret.own_entries_ = true;
  ret.permutation_position_ = permutation_position_;
  ret.permutation_count_ = permutation_count_;
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

std::string Solution::DebugString() const {
  if (entries_ == nullptr) return "<invalid>";
  if (entries_->size() == 0) return "<empty>";

  return absl::StrJoin(*entries_, "\n",
		       [](std::string* out, const Entry& e) {
			 absl::StrAppend(out, e.DebugString());
		       });
}
  
}  // namespace puzzle
