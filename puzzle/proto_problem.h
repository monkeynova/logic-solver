#ifndef PUZZLE_PROTOT_PROBLEM_H
#define PUZZLE_PROTOT_PROBLEM_H

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "puzzle/problem.h"

namespace puzzle {

class ProtoProblem : public Problem {
 public:
  void Setup() override;
  absl::StatusOr<puzzle::Solution> GetSolution() const override;

  virtual void AddPredicates() = 0;
  virtual const google::protobuf::Descriptor* problem_descriptor() const = 0;
  virtual std::string solution_textproto() const = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_PROTOT_PROBLEM_H
