#include "puzzle/problem.h"

namespace puzzle {

// static
Problem* Problem::GetInstance() {
  return global_instance_;
}

// static
Problem* Problem::SetInstance(Problem* p) {
  CHECK(global_instance_ == nullptr);
  global_instance_ = p;
  return global_instance_;
}

Problem* Problem::global_instance_ = nullptr;

}
