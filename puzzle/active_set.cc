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
    builder.AddBlockTo(false, p);
    builder.Add(true);
  }
  if (builder.total() < max_position) {
    builder.AddBlockTo(false, max_position);
  }
  return builder.DoneAdding();
}

ActiveSet ActiveSet::Intersection(const ActiveSet& other) const {
  if (other.is_trivial()) return *this;
  if (is_trivial()) return other;

  ActiveSetIterator this_iterator = Iterator();
  ActiveSetIterator other_iterator = other.Iterator();

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
    } else if (this_iterator.value()) {
      // Single false (other since this is true), it dictactes length.
      next_run_size = other_iterator.run_size();
    } else if (other_iterator.value()) {
      // Single false (this since other is true), it dictactes length.
      next_run_size = this_iterator.run_size();
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
  intersection.AddBlockTo(false, intersection_total);
  return intersection.DoneAdding();
}

std::string ActiveSet::DebugString() const {
  return absl::StrCat("{total:", total_, "; matchs: {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSetBuilder::Add(bool match) {
  ++set_.total_;
  if (match) {
    ++set_.matches_count_;
  }

  if (match == current_value_) {
    ++matches_position_;
  } else {
    set_.matches_.push_back(matches_position_);
    current_value_ = match;
    matches_position_ = 1;
  }
}

void ActiveSetBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;

  set_.total_ += size;
  if (match) {
    set_.matches_count_ += size;
  }

  if (match == current_value_) {
    matches_position_ += size;
  } else {
    set_.matches_.push_back(matches_position_);
    current_value_ = match;
    matches_position_ = size;
  }
}

ActiveSet ActiveSetBuilder::DoneAdding() {
  if (set_.matches_.empty()) {
    CHECK(current_value_) << "skip_match shouldn't be false if skips is empty";
    // As a special case, if all entries are "true", we don't make matches_ so
    // the ActiveSet remains 'trivial'.
    matches_position_ = 0;
    return std::move(set_);
  }
  set_.matches_.push_back(matches_position_);
  current_value_ = true;
  matches_position_ = 0;

  return std::move(set_);
}

std::string ActiveSetIterator::DebugString() const {
  return absl::StrCat("offset: ", offset_, "; ", "total: ", total_, "; ",
                      "value: ", value_, "; ",
                      "match_position: ", match_position_, "; ",
                      "run_position: ", run_position_, "; ", "matches: {",
                      absl::StrJoin(matches_, ","), "}");
}

void ActiveSetIterator::Advance(int n) {
  DCHECK(match_position_ >= matches_.size() ||
         run_position_ != matches_[match_position_])
      << DebugString();
  while (n > 0 && match_position_ < matches_.size()) {
    int delta = matches_[match_position_] - run_position_;
    if (n >= delta) {
      n -= delta;
      offset_ += delta;
      ++match_position_;
      run_position_ = 0;
      value_ = !value_;
    } else {
      run_position_ += n;
      offset_ += n;
      n = 0;
    }
  }
  if (n > 0) {
    offset_ = std::min(total_, offset_ + n);
  }
  if (match_position_ >= matches_.size()) {
    value_ = true;
  }
  DCHECK(match_position_ >= matches_.size() ||
         run_position_ != matches_[match_position_])
      << DebugString();
}

std::vector<int> ActiveSet::EnabledValues() const {
  ActiveSetIterator it = Iterator();
  std::vector<int> ret;
  while (it.more()) {
    int run_size = it.run_size();
    if (it.value()) {
      for (int i = 0; i < run_size; ++i) {
        ret.push_back(i + it.offset());
      }
    }
    it.Advance(run_size);
  }
  return ret;
}

std::string ActiveSet::DebugValues() const {
  return absl::StrCat("{", absl::StrJoin(EnabledValues(), ", "), "}");
}

}  // namespace puzzle
