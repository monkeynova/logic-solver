#include "puzzle/class_permuter_factory.h"

namespace puzzle {

std::unique_ptr<ClassPermuter> MakeClassPermuter(const Descriptor* d,
                                                 int class_int) {
  return absl::make_unique<ClassPermuterFactorialRadixDeleteTracking>(
      d, class_int);
}

}  // namespace puzzle
