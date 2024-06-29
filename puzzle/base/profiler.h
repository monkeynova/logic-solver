#ifndef PUZZLE_BASE_PROFILER_H
#define PUZZLE_BASE_PROFILER_H

#include <memory>

#include "puzzle/base/position.h"

namespace puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() = default;
  virtual void NoteFinish() {}
  virtual bool Done() { return false; }
  virtual double Seconds() { return 0; }

  bool NotePermutation(Position position) {
    if (++permutations_ % 777 != 1) return false;
    return NotePermutationImpl(position);
  }

  void NotePrepare(Position position) {
    if (++prepare_steps_ % 777 != 1) return;
    NotePrepareImpl(position);
  }

 protected:
  Profiler() {}

  virtual bool NotePermutationImpl(Position position) = 0;
  virtual void NotePrepareImpl(Position position) = 0;

  int permutations() const { return permutations_; }
  int prepare_steps() const { return prepare_steps_; }

 private:
  int permutations_ = 0;
  int prepare_steps_ = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_BASE_PROFILER_H
