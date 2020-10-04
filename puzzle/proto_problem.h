#ifndef PUZZLE_PROTOT_PROBLEM_H
#define PUZZLE_PROTOT_PROBLEM_H

#include "google/protobuf/descriptor.h"
#include "puzzle/problem.h"

namespace puzzle {

class ProtoProblem : public Problem {
 public:
  absl::Status Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  virtual absl::Status AddPredicates() = 0;
  virtual const google::protobuf::Descriptor* problem_descriptor() const = 0;
  virtual std::string solution_textproto() const = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_PROTOT_PROBLEM_H
