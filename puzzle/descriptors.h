#ifndef PUZZLE_DESCRIPTORS_H
#define PUZZLE_DESCRIPTORS_H

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include "google/protobuf/descriptor.h"

namespace puzzle {

class Descriptor {
 public:
  virtual ~Descriptor() {}

  virtual int size() const = 0;

  virtual std::string DebugString(int i) const { return absl::StrCat(i); }
};
 
class IntRangeDescriptor : public Descriptor {
 public:
  explicit IntRangeDescriptor(int size) : size_(size) {}
  ~IntRangeDescriptor() override {}

  int size() const override { return size_; }

 private:
  int size_;
};

class StringDescriptor : public Descriptor {
 public:
  StringDescriptor() {}
  ~StringDescriptor() override {}

  void SetDescription(int i, std::string d) {
    CHECK_EQ(i, size_);
    auto pair = names_.emplace(i, std::move(d));
    CHECK(pair.second);
    ++size_;
  }
  std::string DebugString(int i) const override {
    auto it = names_.find(i);
    if (it != names_.end()) return it->second;
    return "";
  }

  int size() const override { return size_; }

 private:
  absl::flat_hash_map<int, std::string> names_;
  int size_ = 0;
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
    id_descriptor_ = id_descriptor;
  }
  void SetClass(int class_int, std::string class_name,
                const Descriptor* name_descriptor) {
    class_descriptor_.SetDescription(class_int, std::move(class_name));
    name_descriptors_.resize(class_int + 1);
    name_descriptors_[class_int] = name_descriptor;
  }

  int num_classes() const { return class_descriptor_.size(); }

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

}  // namespace

#endif  // PUZZLE_DESCRIPTORS_H