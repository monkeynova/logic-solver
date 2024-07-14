#include "puzzle/base/solution_view.h"

namespace puzzle {

Entry Entry::invalid_(-1);

bool SolutionView::operator==(const SolutionView& other) const {
  if (this == &other) {
    return true;
  }
  if (entries_ == nullptr || other.entries_ == nullptr) {
    return entries_ == other.entries_;
  }
  return *entries_ == *other.entries_;
}

}  // namespace puzzle
