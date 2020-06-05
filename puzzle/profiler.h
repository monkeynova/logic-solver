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

  bool NotePermutation(int64_t position, int64_t count) {
    if (++permutations_ % 777 != 1) return false;
    return NotePermutationImpl(position, count);
  }

  void NotePrepare(int64_t position, int64_t count) {
    if (++prepare_steps_ % 777 != 1) return;
    NotePrepareImpl(position, count);
  }

 protected:
  Profiler() {}

  virtual bool NotePermutationImpl(int64_t position, int64_t count) = 0;
  virtual void NotePrepareImpl(int64_t position, int64_t count) = 0;

  int permutations() const { return permutations_; }
  int prepare_steps() const { return prepare_steps_; }

 private:
  int permutations_ = 0;
  int prepare_steps_ = 0;
};

}  // namespace puzzle

#endif
