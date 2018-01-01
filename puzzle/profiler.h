#ifndef PUZZLE_PROFILER_H
#define PUZZLE_PROFILER_H

#include <memory>

namespace puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() {}
  virtual void NoteFinish() {}
  virtual bool Done() {
    return false;
  }
  virtual double Seconds() { return 0; }

  bool NotePosition(double position, double count) {
    if (++test_calls_ % 777 != 1) return false;
    return NotePositionImpl(position, count);
  }

 protected:
  Profiler() {}

  virtual bool NotePositionImpl(double position, double count) {
    return false;
  }

  int test_calls() const {
    return test_calls_;
  }

 private:
  int test_calls_ = 0;
};

}  // namespace puzzle

#endif
