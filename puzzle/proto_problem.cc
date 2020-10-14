#include "puzzle/proto_problem.h"

#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/text_format.h"

namespace puzzle {

static absl::StatusOr<EntryDescriptor> MakeEntryDescriptor(
    const google::protobuf::Descriptor* proto_descriptor) {
  if (proto_descriptor->field_count() != 1) {
    return absl::InvalidArgumentError("Proto must have a single field");
  }
  const google::protobuf::FieldDescriptor* entry_field =
      proto_descriptor->field(0);
  if (entry_field->label() !=
      google::protobuf::FieldDescriptor::Label::LABEL_REPEATED) {
    return absl::InvalidArgumentError("Proto field must be a repeated");
  }
  const google::protobuf::Descriptor* entry_descriptor =
      entry_field->message_type();
  if (entry_descriptor == nullptr) {
    return absl::InvalidArgumentError("Proto field must be of type Message");
  }

  const google::protobuf::FieldDescriptor* id_field =
      entry_descriptor->FindFieldByNumber(1);
  if (id_field == nullptr) {
    return absl::InvalidArgumentError("Entry field must have field number 1");
  }
  if (id_field->name() != "id") {
    return absl::InvalidArgumentError(
        "Entry field with tag 1 must be named id");
  }
  const google::protobuf::EnumDescriptor* id_enum = id_field->enum_type();
  if (id_enum == nullptr) {
    return absl::InvalidArgumentError("id field must be of type enum");
  }

  std::vector<std::string> class_names(entry_descriptor->field_count() - 1);
  std::vector<std::unique_ptr<const Descriptor>> class_descriptors(
      entry_descriptor->field_count() - 1);
  for (int i = 0; i < entry_descriptor->field_count(); ++i) {
    const google::protobuf::FieldDescriptor* field = entry_descriptor->field(i);
    if (field == id_field) {
      continue;
    }
    const google::protobuf::EnumDescriptor* enum_type = field->enum_type();
    if (enum_type == nullptr) {
      return absl::InvalidArgumentError("fields must be of type enum");
    }
    // TODO(@monkeynova): -2 here takes the first non-id feild (2) and maps it
    // to the first class (0). This is an awful, hard-coded translation and a
    // better data model should be found.
    class_names[field->number() - 2] = field->name();
    class_descriptors[field->number() - 2] =
        absl::make_unique<ProtoEnumDescriptor>(enum_type);
  }

  EntryDescriptor puzzle_entry_desriptor(
      absl::make_unique<ProtoEnumDescriptor>(id_enum),
      absl::make_unique<StringDescriptor>(class_names),
      std::move(class_descriptors));

  return puzzle_entry_desriptor;
}

static EntryDescriptor CheckDescriptor(
    absl::StatusOr<EntryDescriptor> entry_descriptor) {
  // TODO(@monkeynova): Maybe find a way to defer until setup...
  CHECK(entry_descriptor.ok()) << entry_descriptor.status();
  return std::move(*entry_descriptor);
}

ProtoProblem::ProtoProblem(
    const google::protobuf::Descriptor* problem_descriptor)
    : Problem(CheckDescriptor(MakeEntryDescriptor(problem_descriptor))),
      problem_descriptor_(problem_descriptor) {}

absl::StatusOr<Solution> ProtoProblem::GetSolution() const {
  google::protobuf::DynamicMessageFactory factory;
  const google::protobuf::Descriptor* proto_descriptor = problem_descriptor();

  std::unique_ptr<google::protobuf::Message> solution =
      absl::WrapUnique(factory.GetPrototype(proto_descriptor)->New());
  if (!google::protobuf::TextFormat::ParseFromString(solution_textproto(),
                                                     solution.get())) {
    return absl::InvalidArgumentError("Could not parse proto");
  }

  if (proto_descriptor->field_count() != 1) {
    return absl::InvalidArgumentError("Proto must have a single field");
  }
  const google::protobuf::FieldDescriptor* entry_field =
      proto_descriptor->field(0);
  if (entry_field->label() !=
      google::protobuf::FieldDescriptor::Label::LABEL_REPEATED) {
    return absl::InvalidArgumentError("Proto field must be a repeated");
  }
  const google::protobuf::Descriptor* entry_proto = entry_field->message_type();
  if (entry_proto == nullptr) {
    return absl::InvalidArgumentError("Proto field must be of type Message");
  }

  const google::protobuf::FieldDescriptor* id_field =
      entry_proto->FindFieldByNumber(1);
  if (id_field == nullptr) {
    return absl::InvalidArgumentError("Entry field must have field number 1");
  }
  if (id_field->name() != "id") {
    return absl::InvalidArgumentError(
        "Entry field with tag 1 must be named id");
  }
  const google::protobuf::EnumDescriptor* id_enum = id_field->enum_type();
  if (id_enum == nullptr) {
    return absl::InvalidArgumentError("id field must be of type enum");
  }

  const google::protobuf::RepeatedFieldRef<
      google::protobuf::Message>& proto_entries =
      solution->GetReflection()->GetRepeatedFieldRef<google::protobuf::Message>(
          *solution, entry_field);

  std::vector<Entry> entries;
  for (const auto& entry : proto_entries) {
    int id = entry.GetReflection()->GetEnumValue(entry, id_field);
    std::vector<int> class_values(entry_descriptor()->AllClasses()->size(), 0);
    for (int i = 0; i < entry_proto->field_count(); ++i) {
      const google::protobuf::FieldDescriptor* field = entry_proto->field(i);
      if (field == id_field) {
        continue;
      }
      const google::protobuf::EnumDescriptor* enum_type = field->enum_type();
      if (enum_type == nullptr) {
        return absl::InvalidArgumentError("fields must be of type enum");
      }
      // TODO(@monkeynova): -2 here takes the first non-id feild (2) and maps it
      // to the first class (0). This is an awful, hard-coded translation and a
      // better data model should be found.
      class_values[field->number() - 2] =
          entry.GetReflection()->GetEnumValue(entry, field);
    }
    entries.emplace_back(id, std::move(class_values), entry_descriptor());
  }

  return Solution(entry_descriptor(), &entries).Clone();
}

absl::Status ProtoProblem::Setup() { return AddPredicates(); }

}  // namespace puzzle
