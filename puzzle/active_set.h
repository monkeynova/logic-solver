#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include "puzzle/active_set_bit_vector.h"
#include "puzzle/active_set_run_length.h"

namespace puzzle {

using ActiveSetIterator = ActiveSetRunLengthIterator;
using ActiveSet = ActiveSetRunLength;
using ActiveSetBuilder = ActiveSetRunLengthBuilder;

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
