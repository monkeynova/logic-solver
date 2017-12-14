#ifndef PUZZLE_PROFILER_H
#define PUZZLE_PROFILER_H

#include <memory>

namespace puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() {}
  virtual bool NotePosition(double position, double count) {
    return false;
  }
  virtual void NoteFinish() {}
  virtual bool Done() {
    return false;
  }
  virtual double Seconds() { return 0; }
  
 protected:
  Profiler() {}
};

}  // namespace puzzle

#endif
