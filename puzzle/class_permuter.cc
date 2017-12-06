#include "puzzle/class_permuter.h"

namespace Puzzle {

void ClassPermuter::iterator::BuildCurrent() {
  if (position_ >= max_) {
    current_.resize(0);
  } else {
    int tmp = position_;
    for (unsigned int i = 0; i < current_.size(); ++i) {
      current_[i] = values_[i];
    }
    for (unsigned int i = 0; tmp && i < current_.size(); ++i) {
      int next = tmp % (current_.size() - i);
      tmp /= (current_.size() - i);
      std::swap(current_[i], current_[i + next]);
    }
  }
}

// static
double ClassPermuter::PermutationCount(const Descriptor* d) {
  if (d == nullptr) return 0;
  
  double ret = 1;
  int value_count = d->Values().size();
  for (int i = 2; i <= value_count; i++ ) {
    ret *= i;
  }
  return ret;
}

}  // namespace Puzzle
