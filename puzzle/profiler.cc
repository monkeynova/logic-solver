#include "puzzle/profiler.h"

#include <iostream>
#include <sys/time.h>

#include "absl/memory/memory.h"

namespace Puzzle {

namespace {

class StructTimevalProfiler : public Profiler {
 public:
  StructTimevalProfiler() : Profiler() {
    gettimeofday(&start_, nullptr);
  }
  ~StructTimevalProfiler() override {
    std::cout << "\033[1K\r" << std::flush;
  }
  
 private:
  void NotePosition(double position, double count) override {
    if (++test_calls_ % 77 != 1) return;

    struct timeval now;
    gettimeofday(&now, nullptr);
    double delta =
        (now.tv_sec - last_.tv_sec + 1e-6 * (now.tv_usec - last_.tv_usec));
   if (delta < 0.1) return;
    
    double qps = (position - last_position_) / delta;
    std::cout << "\033[1K\rTrying " << (100 * position / count) << "%, "
              << qps/1000 << "Kqps" << std::flush;
    last_ = now;
    last_position_ = position;
  }
  
  struct timeval start_;
  struct timeval last_;
  double last_position_;
  int test_calls_ = 0;
};

class NoopProfiler : public Profiler {
 public:
  NoopProfiler() : Profiler() {}

 private:
  void NotePosition(double position, double count) override {}
};

}  // namespace

std::unique_ptr<Profiler> Profiler::Create() {
  return absl::make_unique<StructTimevalProfiler>();
}

}  // namespace Puzzle
