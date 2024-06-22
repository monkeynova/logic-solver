#ifndef PUZZLE_BASE_SOLUTION_H
#define PUZZLE_BASE_SOLUTION_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "puzzle/base/descriptors.h"

namespace puzzle {

class Entry {
 public:
  using Predicate = std::function<bool(const Entry&)>;

  static constexpr int kBadId = std::numeric_limits<int>::max();

  Entry(int id, const std::vector<int>& classes,
        const EntryDescriptor* entry_descriptor)
      : id_(id), classes_(classes), entry_descriptor_(entry_descriptor) {}
  ~Entry() {}

  bool operator==(const Entry& other) const {
    if (this == &other) {
      return true;
    } else if (id_ != other.id_) {
      return false;
    } else {
      return classes_ == other.classes_;
    }
  }

  int id() const { return id_; }
  int Class(int classname) const { return classes_[classname]; }
  void SetClass(int classname, int value) { classes_[classname] = value; }

  static const Entry& Invalid() { return invalid_; }
  const EntryDescriptor* descriptor() const { return entry_descriptor_; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Entry& e) {
    if (e.entry_descriptor_ != nullptr) {
      absl::Format(&sink, "%v", e.entry_descriptor_->Id(e.id_));
    } else {
      absl::Format(&sink, "%v", e.id_);
    }
    absl::Format(&sink, ":");
    for (unsigned int i = 0; i < e.classes_.size(); ++i) {
      if (e.entry_descriptor_) {
        absl::Format(&sink, " %v=%v", e.entry_descriptor_->Class(i),
                     e.entry_descriptor_->Name(i, e.classes_[i]));
      } else {
        absl::Format(&sink, " %v", e.classes_[i]);
      }
    }
  }

 private:
  Entry(int id) : id_(id), entry_descriptor_(nullptr) {}

  int id_;
  std::vector<int> classes_;
  const EntryDescriptor* entry_descriptor_;
  static Entry invalid_;
};

class Solution {
 public:
  struct Position {
    double position;
    double count;
    double Completion() const {
      if (count == 0) return 0;
      return position / count;
    }
    template <typename Sink>
    friend void AbslStringify(Sink& sink, const Position& p) {
      absl::Format(&sink, "%e/%e (%f)", p.position, p.count, p.Completion());
    }
  };

  using Predicate = std::function<bool(const Solution&)>;

  Solution() {}
  Solution(const EntryDescriptor* entry_descriptor,
           const std::vector<Entry>* entries)
      : entry_descriptor_(entry_descriptor), entries_(entries) {}

  ~Solution() {
    if (own_entries_ && entries_ != nullptr) {
      delete entries_;
    }
  }

  Solution(const Solution& other) = delete;
  Solution& operator=(const Solution& other) = delete;

  Solution(Solution&& other) { *this = std::move(other); }
  Solution& operator=(Solution&& other);

  Solution Clone() const;
  bool operator==(const Solution& other) const;

  const EntryDescriptor* descriptor() const { return entry_descriptor_; }

  Position position() const { return permutation_position_; }
  void set_position(Position position) { permutation_position_ = position; }

  bool IsValid() const { return entries_ != nullptr; }
  const std::vector<Entry>& entries() const { return *entries_; }
  const Entry& Id(int id) const { return (*entries_)[id]; }
  const Entry& Find(Entry::Predicate pred) const {
    for (const Entry& e : *entries_) {
      if (pred(e)) {
        return e;
      }
    }
    LOG(ERROR) << "Cannot find an entry for the given predicate";
    return Entry::Invalid();
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Solution& s) {
    if (s.entries_ == nullptr)
      absl::Format(&sink, "<invalid>");
    else if (s.entries_->size() == 0)
      absl::Format(&sink, "<empty>");
    else {
      bool first = true;
      for (const Entry& e : *s.entries_) {
        if (first)
          first = false;
        else
          absl::Format(&sink, "\n");
        absl::Format(&sink, "%v", e);
      }
    }
  }
  friend std::ostream& operator<<(std::ostream& o, const Solution& solution) {
    return o << absl::StreamFormat("%v", solution);
  }

 private:
  const EntryDescriptor* entry_descriptor_ = nullptr;  // Not owned

  const std::vector<Entry>* entries_ = nullptr;

  // If true, 'this' owns 'entries_'.
  bool own_entries_ = false;

  // The position of in iterating through all permutations of solutions which
  // this represents.
  Position permutation_position_ = {.position = 0, .count = 0};

  // The total number of all permutations that can be generated from
  // 'entry_descriptor_'.
  double permutation_count_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_BASE_SOLUTION_H