#ifndef PUZZLE_DESCRIPTORS_H
#define PUZZLE_DESCRIPTORS_H

#include "absl/container/flat_hash_map.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "google/protobuf/descriptor.h"
#include "vlog.h"

namespace puzzle {

class Descriptor {
 public:
  virtual ~Descriptor() {}

  virtual int size() const = 0;

  virtual std::string DebugString(int i) const = 0;
};

class IntRangeDescriptor : public Descriptor {
 public:
  explicit IntRangeDescriptor(int size) : size_(size) {}
  ~IntRangeDescriptor() override {}

  int size() const override { return size_; }

  std::string DebugString(int i) const override { return absl::StrCat(i); }

 private:
  int size_;
};

class StringDescriptor : public Descriptor {
 public:
  explicit StringDescriptor(std::vector<std::string> names)
      : names_(std::move(names)) {}
  ~StringDescriptor() override {}

  std::string DebugString(int i) const override {
    if (i >= names_.size() || i < 0) return "";
    return names_[i];
  }

  int size() const override { return names_.size(); }

 private:
  std::vector<std::string> names_;
};

class ProtoEnumDescriptor : public StringDescriptor {
 public:
  ProtoEnumDescriptor(const google::protobuf::EnumDescriptor* proto_descriptor)
      : StringDescriptor(ProtoNames(proto_descriptor)) {}

 private:
  static std::vector<std::string> ProtoNames(
      const google::protobuf::EnumDescriptor* proto_descriptor) {
    std::vector<std::string> names(proto_descriptor->value_count());
    for (int i = 0; i < proto_descriptor->value_count(); ++i) {
      names[proto_descriptor->value(i)->number()] =
          proto_descriptor->value(i)->name();
    }
    return names;
  }
};

class EntryDescriptor {
 public:
  EntryDescriptor(
      std::unique_ptr<const Descriptor> id_descriptor,
      std::unique_ptr<const Descriptor> class_descriptor,
      std::vector<std::unique_ptr<const Descriptor>> name_descriptors)
      : id_descriptor_(std::move(id_descriptor)),
        class_descriptor_(std::move(class_descriptor)),
        name_descriptors_(std::move(name_descriptors)) {}

  int num_classes() const { return class_descriptor_->size(); }

  const Descriptor* AllIds() const { return id_descriptor_.get(); }
  const Descriptor* AllClasses() const { return class_descriptor_.get(); }
  const Descriptor* AllClassValues(int class_int) const {
    return name_descriptors_[class_int].get();
  }

  std::string Id(int id_int) const {
    return id_descriptor_->DebugString(id_int);
  }
  std::string Class(int class_int) const {
    return class_descriptor_->DebugString(class_int);
  }
  std::string Name(int class_int, int name_int) const {
    return name_descriptors_[class_int]
               ? name_descriptors_[class_int]->DebugString(name_int)
               : "";
  }

 private:
  std::unique_ptr<const Descriptor> id_descriptor_;
  std::unique_ptr<const Descriptor> class_descriptor_;
  std::vector<std::unique_ptr<const Descriptor>> name_descriptors_;
};

}  // namespace puzzle

#endif  // PUZZLE_DESCRIPTORS_H