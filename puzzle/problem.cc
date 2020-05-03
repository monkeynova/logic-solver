#include "puzzle/problem.h"

namespace puzzle {

// static
std::unique_ptr<Problem> Problem::GetInstance() {
  return global_problem_generator_();
}

Problem::Generator Problem::SetGenerator(Generator generator) {
  return global_problem_generator_ = generator;
}

std::unique_ptr<Problem> Problem::MissingGenerator() {
  CHECK(false) << "No Problem Registered";
  return nullptr;
}

Problem::Generator Problem::global_problem_generator_ = &MissingGenerator;

}  // namespace puzzle
