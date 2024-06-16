#ifndef PROTO_PROTO_PROBLEM_H
#define PROTO_PROTO_PROBLEM_H

#include "google/protobuf/descriptor.h"
#include "puzzle/problem.h"

namespace puzzle {

class ProtoProblemBase : public Problem {
 public:
  explicit ProtoProblemBase(
      const google::protobuf::Descriptor* problem_descriptor)
    : ProtoProblemBase(MakeInit(problem_descriptor)) {}

  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  virtual absl::Status AddPredicates() = 0;
  const google::protobuf::Descriptor* problem_descriptor() const {
    return problem_descriptor_;
  }
  virtual std::string solution_textproto() const = 0;

 private:
  struct Init {
    const google::protobuf::Descriptor* problem_descriptor;
    EntryDescriptor entry_descriptor;
    const google::protobuf::FieldDescriptor* entry_field;
    const google::protobuf::Descriptor* entry_message_descriptor;
    const google::protobuf::FieldDescriptor* id_field;
    std::vector<const google::protobuf::FieldDescriptor*> class_fields;
  };
  explicit ProtoProblemBase(absl::StatusOr<ProtoProblemBase::Init> init);

  static absl::StatusOr<Init> MakeInit(
      const google::protobuf::Descriptor* proto_descriptor);

  const google::protobuf::Descriptor* problem_descriptor_;
  const google::protobuf::FieldDescriptor* entry_field_;
  const google::protobuf::Descriptor* entry_message_descriptor_;
  const google::protobuf::FieldDescriptor* id_field_;
  std::vector<const google::protobuf::FieldDescriptor*> class_fields_;
};

template <class Proto>
class ProtoProblem : public ProtoProblemBase {
 public:
  ProtoProblem() : ProtoProblemBase(Proto::descriptor()) {}
};

}  // namespace puzzle

#endif  // PROTO_PROTO_PROBLEM_H
