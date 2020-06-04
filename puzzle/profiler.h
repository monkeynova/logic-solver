#ifndef PUZZLE_PROFILER_H
#define PUZZLE_PROFILER_H

#include <memory>

namespace puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() {}
  virtual void NoteFinish() {}
  virtual bool Done() { return false; }
  virtual double Seconds() { return 0; }

  bool NotePermutation(double position, double count) {
    if (++permutations_ % 777 != 1) return false;
    return NotePermutationImpl(position, count);
  }

 protected:
  Profiler() {}

  virtual bool NotePermutationImpl(double position, double count) {
    return false;
  }

  int permutations() const { return permutations_; }

 private:
  int permutations_ = 0;
};

}  // namespace puzzle

#endif
