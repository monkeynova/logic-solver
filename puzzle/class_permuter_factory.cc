#include "puzzle/class_permuter_factory.h"

#include "puzzle/class_permuter_factorial_radix.h"
#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

namespace puzzle {

std::unique_ptr<ClassPermuter> MakeClassPermuter(const Descriptor* d,
                                                 int class_int) {
  return absl::make_unique<ClassPermuterFactorialRadixDeleteTracking>(
      d, class_int);
}

}  // namespace puzzle
