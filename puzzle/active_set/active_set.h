#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include "puzzle/active_set/bit_vector.h"
#include "puzzle/active_set/run_length.h"
#include "puzzle/active_set/run_position.h"

namespace puzzle {

#ifdef ACTIVE_SET_BIT_VECTOR
using ActiveSet = ActiveSetBitVector;
#else
#ifdef ACTIVE_SET_RUN_LENGTH
using ActiveSet = ActiveSetRunLength;
#else
using ActiveSet = ActiveSetRunPosition;
#endif
#endif

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
