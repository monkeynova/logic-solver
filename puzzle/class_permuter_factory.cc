#include "puzzle/class_permuter_factory.h"

#include "absl/flags/flag.h"
#include "puzzle/class_permuter_factorial_radix.h"
#include "puzzle/class_permuter_factorial_radix_delete_tracking.h"
#include "puzzle/class_permuter_steinhaus_johnson_trotter.h"

enum class ClassPermuterType {
			      kUnknown = 0,
			      kFactorialRadix = 1,
			      kFactorialRadixDeleteTracking = 2,
			      kSteinhausJohnsonTrotter = 3,
};

ABSL_FLAG(ClassPermuterType, puzzle_class_permuter_type, ClassPermuterType::kFactorialRadixDeleteTracking, "...");

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
  return absl::StrCat("Unknown ClassPermuterType(", static_cast<int>(class_permuter_type), ")");
}

bool AbslParseFlag(absl::string_view value, ClassPermuterType* class_permuter_type, std::string* error) {
  if (value == "factorial_radix") {
    *class_permuter_type = ClassPermuterType::kFactorialRadix;
    return true;
  } else if (value == "delete_tracking") {
    *class_permuter_type = ClassPermuterType::kFactorialRadixDeleteTracking;
    return true;
  } else if ("steinhaus_johnson_trotter") {
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
    return absl::make_unique<ClassPermuterFactorialRadix>(
      d, class_int);
  case ClassPermuterType::kFactorialRadixDeleteTracking:
    return absl::make_unique<ClassPermuterFactorialRadixDeleteTracking>(
      d, class_int);
  case ClassPermuterType::kSteinhausJohnsonTrotter:
    return absl::make_unique<ClassPermuterSteinhausJohnsonTrotter>(
      d, class_int);
  case ClassPermuterType::kUnknown:
    // Fallthrough intended.
    ;
  }

  LOG(FATAL) << "Unknown ClassPermuterType(" << static_cast<int>(absl::GetFlag(FLAGS_puzzle_class_permuter_type)) << ")";
  return nullptr;
}

}  // namespace puzzle
