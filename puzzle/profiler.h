#ifndef PUZZLE_PROFILER_H

#include <memory>

namespace Puzzle {

class Profiler {
 public:
  static std::unique_ptr<Profiler> Create();

  virtual ~Profiler() {}
  virtual void NotePosition(double position, double count) = 0;
  
 protected:
  Profiler() {}
};

}  // namespace Puzzle

#endif
