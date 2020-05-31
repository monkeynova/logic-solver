#ifndef PUZZLE_ACTIVE_SET_H
#define PUZZLE_ACTIVE_SET_H

#ifdef ACTIVE_SET_BIT_VECTOR
#include "puzzle/active_set_bit_vector.h"
#else
#include "puzzle/active_set_run_length.h"
#endif

namespace puzzle {

#ifdef ACTIVE_SET_BIT_VECTOR
using ActiveSet = ActiveSetBitVector;
#else
using ActiveSet = ActiveSetRunLength;
#endif

using ActiveSetIterator = ActiveSet::Iterator;
using ActiveSetBuilder = ActiveSet::Builder;
 
}  // namespace puzzle

#endif  // PUZZLE_ACTIVE_SET_H
