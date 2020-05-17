#ifndef PUZZLE_CLASS_PERMUTER_FACTORY_H
#define PUZZLE_CLASS_PERMUTER_FACTORY_H

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"

namespace puzzle {

std::unique_ptr<ClassPermuter> MakeClassPermuter(const Descriptor* d = nullptr,
                                                 int class_int = 0);

}  // namespace puzzle

#endif  // PUZZLE_CLASS_PERMUTER_FACTORY_H
