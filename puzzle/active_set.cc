#include "puzzle/active_set.h"

#include <algorithm>
#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace puzzle {

namespace {

std::vector<int> SortFlatHashSet(const absl::flat_hash_set<int>& unsorted) {
  std::vector<int> sorted;
  sorted.reserve(unsorted.size());
  std::copy(unsorted.begin(), unsorted.end(), std::back_inserter(sorted));
  std::sort(sorted.begin(), sorted.end());
  return sorted;
}

}  // namespace

// static
ActiveSet ActiveSetBuilder::FromPositions(
    const absl::flat_hash_set<int>& positions, int max_position) {
  return FromPositions(SortFlatHashSet(positions), max_position);
}

// static
ActiveSet ActiveSetBuilder::FromPositions(
    const std::initializer_list<int>& positions, int max_position) {
  return FromPositions(absl::flat_hash_set<int>(positions), max_position);
}

// static
ActiveSet ActiveSetBuilder::FromPositions(const std::vector<int>& positions,
                                          int max_position) {
  ActiveSetBuilder builder;
  for (auto p : positions) {
    if (p < 0) continue;
    if (p >= max_position) break;
    builder.AddBlock(false, p - builder.total());
    builder.Add(true);
  }
  if (builder.total() < max_position) {
    builder.AddBlock(false, max_position - builder.total());
  }
  return builder.DoneAdding();
}

struct ActiveSetIterator {
  explicit ActiveSetIterator(const std::vector<int>& matches)
      : matches_(matches) {}

  bool value() const { return value_; }

  bool more() const {
    return match_position_ < static_cast<int>(matches_.size());
  }

  int run_size() const { return matches_[match_position_] - run_position_; }

  void Advance(int n) {
    run_position_ += n;
    while (more() && run_position_ >= matches_[match_position_]) {
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

ActiveSet ActiveSet::Intersection(const ActiveSet& other) const {
  if (other.is_trivial()) return *this;
  if (is_trivial()) return other;

  ActiveSetIterator this_iterator(matches_);
  ActiveSetIterator other_iterator(other.matches_);

  VLOG(3) << "Intersect(" << DebugString() << ", " << other.DebugString()
          << ")";

  ActiveSetBuilder intersection;
  while (this_iterator.more() && other_iterator.more()) {
    VLOG(3) << "Intersect NextBlock="
            << (this_iterator.value() ? "true" : "false") << "/\\"
            << (other_iterator.value() ? "true" : "false");
    bool next_run_value = false;
    int next_run_size;
    if (this_iterator.value() && other_iterator.value()) {
      // Both true, so true for run-length min.
      next_run_value = true;
      next_run_size =
          std::min(this_iterator.run_size(), other_iterator.run_size());
    } else if (!this_iterator.value()) {
      // Single false, it dictactes length.
      next_run_size = this_iterator.run_size();
    } else if (!other_iterator.value()) {
      // Single false, it dictactes length.
      next_run_size = other_iterator.run_size();
    } else {
      // Both false, so false for run-length max.
      next_run_size =
          std::max(this_iterator.run_size(), other_iterator.run_size());
    }
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

  const int intersection_total = std::max(total_, other.total_);
  intersection.AddBlock(false, intersection_total - intersection.total() - 1);
  // Mark done and update self.
  return intersection.DoneAdding();
}

std::string ActiveSet::DebugString() const {
  return absl::StrCat("{", (current_value_ ? "match" : "skip"), " ",
                      matches_position_, " (of ", total_, ") {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSetBuilder::Add(bool match) {
  ++set_.total_;
  if (match) {
    ++set_.matches_count_;
  }

  if (match == set_.current_value_) {
    ++set_.matches_position_;
  } else {
    set_.matches_.push_back(set_.matches_position_);
    set_.current_value_ = match;
    set_.matches_position_ = 1;
  }
}

void ActiveSetBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;

  set_.total_ += size;
  if (match) {
    set_.matches_count_ += size;
  }

  if (match == set_.current_value_) {
    set_.matches_position_ += size;
  } else {
    set_.matches_.push_back(set_.matches_position_);
    set_.current_value_ = match;
    set_.matches_position_ = size;
  }
}

ActiveSet ActiveSetBuilder::DoneAdding() {
  if (set_.matches_.empty()) {
    CHECK(set_.current_value_)
        << "skip_match shouldn't be false if skips is empty";
    // As a special case, if all entries are "true", we don't make matches_ so
    // the ActiveSet remains 'trivial'.
    set_.matches_position_ = 0;
    return std::move(set_);
  }
  set_.matches_.push_back(set_.matches_position_);
  set_.current_value_ = true;
  set_.matches_position_ = 0;

  return std::move(set_);
}

bool ActiveSet::ConsumeNext() {
  if (matches_.empty()) return true;
  if (matches_position_ >= static_cast<int>(matches_.size())) return true;

  if (matches_[matches_position_] == 0) {
    current_value_ = !current_value_;
    ++matches_position_;
    if (matches_position_ >= static_cast<int>(matches_.size())) return true;
  }
  --matches_[matches_position_];
  return current_value_;
}

int ActiveSet::ConsumeFalseBlock() {
  if (matches_.empty()) return 0;
  if (matches_position_ >= static_cast<int>(matches_.size())) return 0;

  if (matches_[matches_position_] == 0) {
    current_value_ = !current_value_;
    ++matches_position_;
    if (matches_position_ >= static_cast<int>(matches_.size())) return 0;
  }
  if (current_value_) return 0;

  int ret = matches_[matches_position_];
  ++matches_position_;
  current_value_ = true;
  return ret;
}

bool ActiveSet::DiscardBlock(int block_size) {
  if (matches_.empty()) return true;

  while (block_size > 0) {
    if (matches_[matches_position_] == 0) {
      current_value_ = !current_value_;
      ++matches_position_;
      if (matches_position_ >= static_cast<int>(matches_.size())) return true;
    }
    int delta = std::min(matches_[matches_position_], block_size);
    matches_[matches_position_] -= delta;
    block_size -= delta;
  }
  return current_value_;
}

std::vector<int> ActiveSet::EnabledValues() const {
  ActiveSet copy = *this;
  std::vector<int> ret;
  for (int i = 0; i < copy.total(); ++i) {
    if (copy.ConsumeNext()) {
      ret.push_back(i);
    }
  }
  return ret;
}

std::string ActiveSet::DebugValues() const {
  return absl::StrCat("{", absl::StrJoin(EnabledValues(), ", "), "}");
}

}  // namespace puzzle
