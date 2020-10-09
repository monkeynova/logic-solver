#ifndef PUZZLE_PROTOT_PROBLEM_H
#define PUZZLE_PROTOT_PROBLEM_H

#include "google/protobuf/descriptor.h"
#include "puzzle/problem.h"

namespace puzzle {

class ProtoProblem : public Problem {
 public:
  explicit ProtoProblem(const google::protobuf::Descriptor* problem_descriptor);

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

}  // namespace puzzle

#endif  // PUZZLE_PROTOT_PROBLEM_H
