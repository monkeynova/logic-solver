#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#include "puzzle/active_set_bit_vector.h"
#include "puzzle/active_set_run_length.h"
#include "puzzle/active_set_run_position.h"

namespace puzzle {

#ifdef ACTIVE_SET_BIT_VECTOR
using ActiveSet = ActiveSetBitVector;
#else
#ifdef ACTIVE_SET_RUN_POSITION
using ActiveSet = ActiveSetRunPosition;
#else
using ActiveSet = ActiveSetRunLength;
#endif
#endif

using ActiveSetIterator = ActiveSet::Iterator;
using ActiveSetBuilder = ActiveSet::Builder;

}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
