#ifndef PUZZLE_SOLUTION_H
#define PUZZLE_SOLUTION_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include "google/protobuf/descriptor.h"

namespace puzzle {

class Descriptor {
 public:
  virtual ~Descriptor() {}

  virtual std::vector<int> Values() const = 0;

  virtual std::string DebugString(int i) const { return absl::StrCat(i); }
};

class IntRangeDescriptor : public Descriptor {
 public:
  IntRangeDescriptor(int start, int end) : start_(start), end_(end) {}
  ~IntRangeDescriptor() override {}

  std::vector<int> Values() const override {
    std::vector<int> ret;
    ret.reserve(end_ - start_);
    for (int i = start_; i <= end_; ++i) {
      ret.push_back(i);
    }
    return ret;
  }

 private:
  int start_;
  int end_;
};

class StringDescriptor : public Descriptor {
 public:
  StringDescriptor() {}
  ~StringDescriptor() override {}

  void SetDescription(int i, std::string d) { names_[i] = std::move(d); }
  std::string DebugString(int i) const override {
    auto it = names_.find(i);
    if (it != names_.end()) return it->second;
    return "";
  }

  std::vector<int> Values() const override {
    std::vector<int> ret;
    ret.reserve(names_.size());
    for (unsigned int i = 0; i < names_.size(); ++i) {
      ret.push_back(i);
    }
    return ret;
  }

 private:
  absl::flat_hash_map<int, std::string> names_;
};

class ProtoEnumDescriptor : public StringDescriptor {
 public:
  ProtoEnumDescriptor(
      const google::protobuf::EnumDescriptor* proto_descriptor) {
    for (int i = 0; i < proto_descriptor->value_count(); ++i) {
      SetDescription(proto_descriptor->value(i)->number(),
                     proto_descriptor->value(i)->name());
    }
  }
};

class EntryDescriptor {
 public:
  EntryDescriptor() {}

  void SetIds(const Descriptor* id_descriptor) {
    // TODO(@monkeynova): Verify descriptor covers values 0..N.
    id_descriptor_ = id_descriptor;
  }
  void SetClass(int class_int, std::string class_name,
                const Descriptor* name_descriptor) {
    class_descriptor_.SetDescription(class_int, std::move(class_name));
    name_descriptors_.resize(class_int + 1);
    name_descriptors_[class_int] = name_descriptor;
  }

  int num_classes() const { return class_descriptor_.Values().size(); }

  const Descriptor* AllIds() const { return id_descriptor_; }
  const Descriptor* AllClasses() const { return &class_descriptor_; }
  const Descriptor* AllClassValues(int class_int) const {
    return name_descriptors_[class_int];
  }

  std::string Id(int id_int) const {
    return id_descriptor_->DebugString(id_int);
  }
  std::string Class(int class_int) const {
    return class_descriptor_.DebugString(class_int);
  }
  std::string Name(int class_int, int name_int) const {
    return name_descriptors_[class_int]
               ? name_descriptors_[class_int]->DebugString(name_int)
               : "";
  }

 private:
  const Descriptor* id_descriptor_;
  StringDescriptor class_descriptor_;
  std::vector<const Descriptor*> name_descriptors_;
};

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
  bool IsValid() const { return id_ >= 0; }
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
  const Entry& Find(std::function<bool(const Entry&)> pred) const {
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
