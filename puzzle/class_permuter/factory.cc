#include "puzzle/class_permuter/factory.h"

#include "absl/flags/flag.h"
#include "puzzle/class_permuter/factorial_radix.h"
#include "puzzle/class_permuter/factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter/steinhaus_johnson_trotter.h"

enum class ClassPermuterType {
  kUnknown = 0,
  kFactorialRadix = 1,
  kFactorialRadixDeleteTracking = 2,
  kSteinhausJohnsonTrotter = 3,
};

ABSL_FLAG(ClassPermuterType, puzzle_class_permuter_type,
          ClassPermuterType::kFactorialRadixDeleteTracking,
          "Specifies the algorithm to use for computing all class "
          "permutations. The algorithm choice affects the performance of "
          "calculating the permutations as well as the ordering in which "
          "permutations are produced, which in turn can further affect "
          "the performance of filtering behaviors. Value values are "
          "\"factorial_radix\", \"delete_tracking\", and "
          "\"steinhaus_johnson_trotter\".");

std::string AbslUnparseFlag(ClassPermuterType class_permuter_type) {
  switch (class_permuter_type) {
    case ClassPermuterType::kFactorialRadix:
      return "factorial_radix";
    case ClassPermuterType::kFactorialRadixDeleteTracking:
      return "delete_tracking";
    case ClassPermuterType::kSteinhausJohnsonTrotter:
      return "steinhaus_johnson_trotter";
    case ClassPermuterType::kUnknown:
        // Fallthrough intended.
        ;
  }
  return absl::StrCat("Unknown ClassPermuterType(",
                      static_cast<int>(class_permuter_type), ")");
}

bool AbslParseFlag(absl::string_view value,
                   ClassPermuterType* class_permuter_type, std::string* error) {
  if (value == "factorial_radix") {
    *class_permuter_type = ClassPermuterType::kFactorialRadix;
    return true;
  } else if (value == "delete_tracking") {
    *class_permuter_type = ClassPermuterType::kFactorialRadixDeleteTracking;
    return true;
  } else if (value == "steinhaus_johnson_trotter") {
    *class_permuter_type = ClassPermuterType::kSteinhausJohnsonTrotter;
    return true;
  }
  *error = absl::StrCat("Unknown ClassPermuterType(", value, ")");
  return false;
}

namespace puzzle {

std::unique_ptr<ClassPermuter> MakeClassPermuter(const Descriptor* d,
                                                 int class_int) {
  switch (absl::GetFlag(FLAGS_puzzle_class_permuter_type)) {
    case ClassPermuterType::kFactorialRadix:
      return MakeClassPermuterFactorialRadix()(d->size(), class_int);
    case ClassPermuterType::kFactorialRadixDeleteTracking:
      return MakeClassPermuterFactorialRadixDeleteTracking()(d->size(),
                                                             class_int);
    case ClassPermuterType::kSteinhausJohnsonTrotter:
      return MakeClassPermuterSteinhausJohnsonTrotter()(d->size(), class_int);
    case ClassPermuterType::kUnknown:
        // Fallthrough intended.
        ;
  }

  LOG(FATAL) << "Unknown ClassPermuterType("
             << static_cast<int>(
                    absl::GetFlag(FLAGS_puzzle_class_permuter_type))
             << ")";
  return nullptr;
}

}  // namespace puzzle
