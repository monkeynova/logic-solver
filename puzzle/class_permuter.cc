#include "puzzle/class_permuter.h"

namespace Puzzle {

ClassPermuter::iterator::iterator(const Descriptor* descriptor) {
  if (descriptor != nullptr) {
    values_ = descriptor->Values();
  }
  int entries = values_.size();
  if (entries > 0) {
    max_ = 1;
    for (int i = 2; i <= entries; i++ ) {
      max_ *= i;
    }
  }
  position_ = 0;
  current_.resize(values_.size());
  index_.resize(values_.size());
  direction_.resize(values_.size());
  for (unsigned int i = 0; i < current_.size(); ++i) {
    current_[i] = values_[i];
    index_[i] = i;
    direction_[i] = i == 0 ? 0 : -1;
  }
  next_from_ = current_.size() - 1;
}

#if 1
// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
void ClassPermuter::iterator::Advance() {
  ++position_;
  if (position_ >= max_) {
    current_.resize(0);
  } else {
    int from = next_from_;
    int to = from + direction_[from];
    std::swap(current_[from], current_[to]);
    std::swap(direction_[from], direction_[to]);
    std::swap(index_[from], index_[to]);
    if (to == 0 || to == current_.size() - 1 ||
	index_[to + direction_[to]] > index_[to]) {
      direction_[to] = 0;
      int max = -1;
      for (int i = 0; i < current_.size(); ++i) {
	if (direction_[i] != 0 && index_[i] > max) {
	  next_from_ = i;
	  max = index_[i];
	}
      }
    } else {
      next_from_ = to;
    }
    if (index_[to] < current_.size() - 1) {
      for (int i = 0; i < current_.size(); ++i) {
        if (index_[i] > index_[to]) {
	  if (i < to) {
	    direction_[i] = 1;
	  } else {
	    direction_[i] = -1;
	  }
	}
	if (index_[i] == current_.size() - 1) {
	  next_from_ = i;
	}
      }
    }
  }
}
#else
void ClassPermuter::iterator::Advance() {
  ++position_;
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
#endif

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
