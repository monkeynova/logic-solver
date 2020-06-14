#include "puzzle/proto_problem.h"

#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/text_format.h"

namespace puzzle {

Solution ProtoProblem::GetSolution() const {
  google::protobuf::DynamicMessageFactory factory;
  const google::protobuf::Descriptor* proto_descriptor = problem_descriptor();

  std::unique_ptr<google::protobuf::Message> solution =
      absl::WrapUnique(factory.GetPrototype(proto_descriptor)->New());
  CHECK(google::protobuf::TextFormat::ParseFromString(solution_textproto(),
                                                      solution.get()));

  CHECK_EQ(proto_descriptor->field_count(), 1);
  const google::protobuf::FieldDescriptor* entry_field =
      proto_descriptor->field(0);
  CHECK_EQ(entry_field->label(),
           google::protobuf::FieldDescriptor::Label::LABEL_REPEATED);
  const google::protobuf::Descriptor* entry_proto = entry_field->message_type();
  CHECK(entry_proto != nullptr);

  const google::protobuf::FieldDescriptor* id_field =
      entry_proto->FindFieldByNumber(1);
  CHECK(id_field != nullptr);
  CHECK_EQ(id_field->name(), "id");
  const google::protobuf::EnumDescriptor* id_enum = id_field->enum_type();
  CHECK(id_enum != nullptr);

  const google::protobuf::RepeatedFieldRef<
      google::protobuf::Message>& proto_entries =
      solution->GetReflection()->GetRepeatedFieldRef<google::protobuf::Message>(
          *solution, entry_field);

  std::vector<Entry> entries;
  for (const auto& entry : proto_entries) {
    int id = entry.GetReflection()->GetEnumValue(entry, id_field);
    std::vector<int> class_values(
        entry_descriptor()->AllClasses()->Values().size(), 0);
    for (int i = 0; i < entry_proto->field_count(); ++i) {
      const google::protobuf::FieldDescriptor* field = entry_proto->field(i);
      if (field == id_field) {
        continue;
      }
      const google::protobuf::EnumDescriptor* enum_type = field->enum_type();
      CHECK(enum_type != nullptr);
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

void ProtoProblem::Setup() {
  const google::protobuf::Descriptor* proto_descriptor = problem_descriptor();
  CHECK_EQ(proto_descriptor->field_count(), 1);
  const google::protobuf::FieldDescriptor* entry_field =
      proto_descriptor->field(0);
  CHECK_EQ(entry_field->label(),
           google::protobuf::FieldDescriptor::Label::LABEL_REPEATED);
  const google::protobuf::Descriptor* entry_descriptor =
      entry_field->message_type();
  CHECK(entry_descriptor != nullptr);

  const google::protobuf::FieldDescriptor* id_field =
      entry_descriptor->FindFieldByNumber(1);
  CHECK(id_field != nullptr);
  CHECK_EQ(id_field->name(), "id");
  const google::protobuf::EnumDescriptor* id_enum = id_field->enum_type();
  CHECK(id_enum != nullptr);
  Descriptor* id_descriptor = AddDescriptor(new ProtoEnumDescriptor(id_enum));
  SetIdentifiers(id_descriptor);

  for (int i = 0; i < entry_descriptor->field_count(); ++i) {
    const google::protobuf::FieldDescriptor* field = entry_descriptor->field(i);
    if (field == id_field) {
      continue;
    }
    const google::protobuf::EnumDescriptor* enum_type = field->enum_type();
    CHECK(enum_type != nullptr);
    Descriptor* descriptor = AddDescriptor(new ProtoEnumDescriptor(enum_type));
    // TODO(@monkeynova): -2 here takes the first non-id feild (2) and maps it
    // to the first class (0). This is an awful, hard-coded translation and a
    // better data model should be found.
    AddClass(field->number() - 2, field->name(), descriptor);
  }

  AddPredicates();
}

}  // namespace puzzle
