#ifndef PUZZLE_PROFILER_H
#define PUZZLE_PROFILER_H

#include <memory>

namespace puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() {}
  virtual void NotePosition(double position, double count) = 0;
  virtual bool Done() {
    return false;
  }
  
 protected:
  Profiler() {}
};

}  // namespace puzzle

#endif
