#ifndef PUZZLE_BASE_SOLUTION_VIEW_H
#define PUZZLE_BASE_SOLUTION_VIEW_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "puzzle/base/descriptors.h"
#include "puzzle/base/position.h"

namespace puzzle {

class Entry {
 public:
  using Predicate = std::function<bool(const Entry&)>;

  static constexpr int kBadId = std::numeric_limits<int>::max();

  Entry(int id, const std::vector<int>& classes) : id_(id), classes_(classes) {}
  ~Entry() {}

  bool operator==(const Entry& other) const = default;

  int id() const { return id_; }
  int Class(int classname) const { return classes_[classname]; }
  void SetClass(int classname, int value) { classes_[classname] = value; }

  static const Entry& Invalid() { return invalid_; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Entry& e) {
    absl::Format(&sink, "%v", e.id_);
    absl::Format(&sink, ":");
    for (unsigned int i = 0; i < e.classes_.size(); ++i) {
      absl::Format(&sink, " %v", e.classes_[i]);
    }
  }

  friend struct WithDescriptor;
  struct WithDescriptor {
    const Entry& entry;
    const EntryDescriptor* descriptor;

    template <typename Sink>
    friend void AbslStringify(Sink& sink, const WithDescriptor& ed) {
      if (ed.descriptor == nullptr) {
        absl::Format(&sink, "%v", ed.entry);
        return;
      }
      absl::Format(&sink, "%v", ed.descriptor->Id(ed.entry.id()));
      absl::Format(&sink, ":");
      for (unsigned int i = 0; i < ed.entry.classes_.size(); ++i) {
        absl::Format(&sink, " %v=%v", ed.descriptor->Class(i),
                     ed.descriptor->Name(i, ed.entry.classes_[i]));
      }
    }
  };

 private:
  Entry(int id) : id_(id) {}

  int id_;
  std::vector<int> classes_;
  static Entry invalid_;
};

class SolutionView {
 public:
  using Predicate = std::function<bool(const SolutionView&)>;

  SolutionView() {}
  SolutionView(const EntryDescriptor* entry_descriptor,
               const std::vector<Entry>* entries)
      : entry_descriptor_(entry_descriptor), entries_(entries) {}

  SolutionView(const SolutionView& other) = default;
  SolutionView& operator=(const SolutionView& other) = default;

  bool operator==(const SolutionView& other) const;

  Position position() const { return permutation_position_; }
  void set_position(Position position) { permutation_position_ = position; }

  const EntryDescriptor* descriptor() const { return entry_descriptor_; }

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
  friend void AbslStringify(Sink& sink, const SolutionView& s) {
    if (s.entries_ == nullptr)
      absl::Format(&sink, "<invalid>");
    else if (s.entries_->size() == 0)
      absl::Format(&sink, "<empty>");
    else {
      bool first = true;
      for (const Entry& e : *s.entries_) {
        if (first) {
          first = false;
        } else {
          absl::Format(&sink, "\n");
        }
        absl::Format(&sink, "%v",
                     Entry::WithDescriptor{e, s.entry_descriptor_});
      }
    }
  }
  friend std::ostream& operator<<(std::ostream& o,
                                  const SolutionView& solution) {
    return o << absl::StreamFormat("%v", solution);
  }

 private:
  const EntryDescriptor* entry_descriptor_ = nullptr;  // Not owned

  const std::vector<Entry>* entries_ = nullptr;

  // The position of in iterating through all permutations of solutions which
  // this represents.
  Position permutation_position_ = {.position = 0, .count = 0};
};

}  // namespace puzzle

#endif  // PUZZLE_BASE_SOLUTION_VIEW_H