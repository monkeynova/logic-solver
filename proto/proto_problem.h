#ifndef PROTO_PROTO_PROBLEM_H
#define PROTO_PROTO_PROBLEM_H

#include "google/protobuf/descriptor.h"
#include "puzzle/problem.h"

namespace puzzle {

class ProtoProblemBase : public Problem {
 public:
  explicit ProtoProblemBase(
      const google::protobuf::Descriptor* problem_descriptor);

  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  virtual absl::Status AddPredicates() = 0;
  const google::protobuf::Descriptor* problem_descriptor() const {
    return problem_descriptor_;
  }
  virtual std::string solution_textproto() const = 0;

 private:
  const google::protobuf::Descriptor* problem_descriptor_;
};

template <class Proto>
class ProtoProblem : public ProtoProblemBase {
 public:
  ProtoProblem() : ProtoProblemBase(Proto::descriptor()) {}
};

}  // namespace puzzle

#endif  // PROTO_PROTO_PROBLEM_H
