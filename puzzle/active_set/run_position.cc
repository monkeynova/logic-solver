#include "puzzle/active_set/run_position.h"

#include <algorithm>
#include <iostream>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "vlog.h"

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
ActiveSetRunPosition ActiveSetRunPositionBuilder::FromPositions(
    const absl::flat_hash_set<int>& positions, int max_position) {
  return FromPositions(SortFlatHashSet(positions), max_position);
}

// static
ActiveSetRunPosition ActiveSetRunPositionBuilder::FromPositions(
    const std::initializer_list<int>& positions, int max_position) {
  return FromPositions(absl::flat_hash_set<int>(positions), max_position);
}

// static
ActiveSetRunPosition ActiveSetRunPositionBuilder::FromPositions(
    const std::vector<int>& positions, int max_position) {
  ActiveSetRunPositionBuilder builder(max_position);
  for (auto p : positions) {
    if (p < 0) continue;
    if (p >= max_position) break;
    builder.AddBlockTo(false, p);
    builder.Add(true);
  }
  builder.AddBlockTo(false, max_position);
  return builder.DoneAdding();
}

ActiveSetRunPosition ActiveSetRunPosition::Intersection(
    const ActiveSetRunPosition& other) const {
  if (other.is_trivial()) return *this;
  if (is_trivial()) return other;

  ActiveSetRunPositionIterator this_iterator = GetIterator();
  ActiveSetRunPositionIterator other_iterator = other.GetIterator();

  VLOG(3) << "Intersect(" << DebugString() << ", " << other.DebugString()
          << ")";

  ActiveSetRunPositionBuilder intersection(std::max(total(), other.total()));
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
          std::min(this_iterator.RunSize(), other_iterator.RunSize());
    } else if (this_iterator.value()) {
      // Single false (other since this is true), it dictactes length.
      next_run_size = other_iterator.RunSize();
    } else if (other_iterator.value()) {
      // Single false (this since other is true), it dictactes length.
      next_run_size = this_iterator.RunSize();
    } else {
      // Both false, so false for run-length max.
      next_run_size =
          std::max(this_iterator.RunSize(), other_iterator.RunSize());
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
    int next_run_size = this_iterator.RunSize();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
            << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    this_iterator.Advance(next_run_size);
  }
  while (other_iterator.more()) {
    bool next_run_value = other_iterator.value();
    int next_run_size = other_iterator.RunSize();
    // Store 'next_run_size' values of 'next_run_value'.
    VLOG(3) << "Intersect.AddBlock(" << (next_run_value ? "true" : "false")
            << ", " << next_run_size << ")";
    intersection.AddBlock(next_run_value, next_run_size);
    other_iterator.Advance(next_run_size);
  }

  const int intersection_total = std::max(total(), other.total());
  intersection.AddBlockTo(false, intersection_total);
  return intersection.DoneAdding();
}

std::string ActiveSetRunPosition::DebugString() const {
  return absl::StrCat("{total:", total_, "; matches: {",
                      absl::StrJoin(matches_, ", "), "}}");
}

void ActiveSetRunPositionBuilder::Add(bool match) {
  if (match) {
    ++set_.matches_count_;
  }

  if (match != current_value_) {
    set_.matches_.push_back(offset_);
    current_value_ = match;
  }

  ++offset_;
}

void ActiveSetRunPositionBuilder::AddBlock(bool match, int size) {
  if (size <= 0) return;

  if (match) {
    set_.matches_count_ += size;
  }

  if (match != current_value_) {
    set_.matches_.push_back(offset_);
    current_value_ = match;
  }

  offset_ += size;
}

ActiveSetRunPosition ActiveSetRunPositionBuilder::DoneAdding() {
  CHECK_EQ(offset_, set_.total_);

  set_.matches_.push_back(offset_);

  return std::move(set_);
}

std::string ActiveSetRunPositionIterator::DebugString() const {
  return absl::StrCat("offset: ", offset_, "; ", "total: ", total_, "; ",
                      "match_position: ", match_position_, "; ", "matches: {",
                      absl::StrJoin(matches_, ","), "}");
}

void ActiveSetRunPositionIterator::Advance(int n) {
  offset_ += n;
  if (offset_ >= total_) {
    match_position_ = matches_.size();
    offset_ = total_;
  } else {
    while (matches_[match_position_] <= offset_) {
      ++match_position_;
    }
  }
}

std::vector<int> ActiveSetRunPosition::EnabledValues() const {
  ActiveSetRunPositionIterator it = GetIterator();
  std::vector<int> ret;
  while (it.more()) {
    int run_size = it.RunSize();
    if (it.value()) {
      for (int i = 0; i < run_size; ++i) {
        ret.push_back(i + it.offset());
      }
    }
    it.Advance(run_size);
  }
  return ret;
}

std::string ActiveSetRunPosition::DebugValues() const {
  return absl::StrCat("{", absl::StrJoin(EnabledValues(), ", "), "}");
}

}  // namespace puzzle
