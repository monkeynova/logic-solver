#include "proto/proto_problem.h"

#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/text_format.h"

namespace puzzle {

// static
absl::StatusOr<ProtoProblemBase::Init> ProtoProblemBase::MakeInit(
    const google::protobuf::Descriptor* proto_descriptor) {
  Init ret = {
      .problem_descriptor = proto_descriptor,
      .entry_descriptor = EntryDescriptor(nullptr, nullptr, {}),
  };

  if (proto_descriptor->field_count() != 1) {
    return absl::InvalidArgumentError("Proto must have a single field");
  }
  ret.entry_field = proto_descriptor->field(0);
  if (ret.entry_field->label() !=
      google::protobuf::FieldDescriptor::Label::LABEL_REPEATED) {
    return absl::InvalidArgumentError("Proto field must be a repeated");
  }
  ret.entry_message_descriptor = ret.entry_field->message_type();
  if (ret.entry_message_descriptor == nullptr) {
    return absl::InvalidArgumentError("Proto field must be of type Message");
  }

  ret.id_field = ret.entry_message_descriptor->FindFieldByNumber(1);
  if (ret.id_field == nullptr) {
    return absl::InvalidArgumentError("Entry field must have field number 1");
  }
  if (ret.id_field->name() != "id") {
    return absl::InvalidArgumentError(
        "Entry field with tag 1 must be named id");
  }
  const google::protobuf::EnumDescriptor* id_enum = ret.id_field->enum_type();
  if (id_enum == nullptr) {
    return absl::InvalidArgumentError("id field must be of type enum");
  }

  int class_field_count = ret.entry_message_descriptor->field_count() - 1;
  std::vector<std::string> class_names(class_field_count);
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors(
      class_field_count);
  ret.class_fields.clear();
  ret.class_fields.reserve(class_field_count);
  for (int i = 0; i < ret.entry_message_descriptor->field_count(); ++i) {
    const google::protobuf::FieldDescriptor* field =
        ret.entry_message_descriptor->field(i);
    if (field == ret.id_field) {
      continue;
    }
    const google::protobuf::EnumDescriptor* enum_type = field->enum_type();
    if (enum_type == nullptr) {
      return absl::InvalidArgumentError("fields must be of type enum");
    }

    class_names[ret.class_fields.size()] = field->name();
    class_descriptors[ret.class_fields.size()] =
        absl::make_unique<ProtoEnumDescriptor>(enum_type);
    ret.class_fields.push_back(field);
  }

  ret.entry_descriptor =
      EntryDescriptor(absl::make_unique<ProtoEnumDescriptor>(id_enum),
                      absl::make_unique<StringDescriptor>(class_names),
                      std::move(class_descriptors));

  return ret;
}

ProtoProblemBase::ProtoProblemBase(absl::StatusOr<ProtoProblemBase::Init> init)
    : Problem(std::move(init->entry_descriptor)),
      problem_descriptor_(init->problem_descriptor),
      entry_field_(init->entry_field),
      entry_message_descriptor_(init->entry_message_descriptor),
      id_field_(init->id_field),
      class_fields_(std::move(init->class_fields)) {}

absl::StatusOr<Solution> ProtoProblemBase::GetSolution() const {
  google::protobuf::DynamicMessageFactory factory;
  const google::protobuf::Descriptor* proto_descriptor = problem_descriptor();

  std::unique_ptr<google::protobuf::Message> solution(
      factory.GetPrototype(proto_descriptor)->New());
  if (!google::protobuf::TextFormat::ParseFromString(solution_textproto(),
                                                     solution.get())) {
    return absl::InvalidArgumentError("Could not parse proto");
  }

  const google::protobuf::RepeatedFieldRef<
      google::protobuf::Message>& proto_entries =
      solution->GetReflection()->GetRepeatedFieldRef<google::protobuf::Message>(
          *solution, entry_field_);

  std::vector<Entry> entries;
  for (const auto& entry : proto_entries) {
    int id = entry.GetReflection()->GetEnumValue(entry, id_field_);
    std::vector<int> class_values(entry_descriptor()->AllClasses()->size(), 0);
    for (int i = 0; i < class_fields_.size(); ++i) {
      class_values[i] =
          entry.GetReflection()->GetEnumValue(entry, class_fields_[i]);
    }
    entries.emplace_back(id, std::move(class_values));
  }

  return Solution(entry_descriptor(), &entries).Clone();
}

absl::Status ProtoProblemBase::Setup() { return AddPredicates(); }

}  // namespace puzzle
