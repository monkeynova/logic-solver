#ifndef PUZZLE_BASE_POSITION_H
#define PUZZLE_BASE_POSITION_H

#include "absl/strings/str_format.h"

namespace puzzle {

struct Position {
  double position;
  double count;
  double Completion() const {
    if (count == 0) return 0;
    return position / count;
  }
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Position& p) {
    absl::Format(&sink, "%e/%e (%f)", p.position, p.count, p.Completion());
  }
};

}  // namespace puzzle

#endif  // PUZZLE_BASE_POSITION_H
