#ifndef PUZZLE_SOLUTION_H
#define PUZZLE_SOLUTION_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "puzzle/descriptors.h"

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
  std::string DebugString() const;

  static const Entry& Invalid() { return invalid_; }
  const EntryDescriptor* descriptor() const { return entry_descriptor_; }

 private:
  Entry(int id) : id_(id), entry_descriptor_(nullptr) {}
  int id_;
  std::vector<int> classes_;
  const EntryDescriptor* entry_descriptor_;
  static Entry invalid_;
};

class Solution {
 public:
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

  double permutation_position() const { return permutation_position_; }
  void set_permutation_position(double position) {
    permutation_position_ = position;
  }

  double permutation_count() const { return permutation_count_; }
  void set_permutation_count(double count) { permutation_count_ = count; }

  double Completion() const {
    return permutation_position_ / permutation_count_;
  }

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
  std::string DebugString() const;

 private:
  const EntryDescriptor* entry_descriptor_ = nullptr;  // Not owned

  const std::vector<Entry>* entries_ = nullptr;

  // If true, 'this' owns 'entries_'.
  bool own_entries_ = false;

  // The position of in iterating through all permutations of solutions which
  // this represents.
  double permutation_position_ = 0;

  // The total number of all permutations that can be generated from
  // 'entry_descriptor_'.
  double permutation_count_ = 0;
};

inline void PrintTo(const Solution& solution, ::std::ostream* os) {
  *os << solution.DebugString();
}

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_H
