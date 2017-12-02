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
    if (++test_calls_ % 777777 != 1) return;
    
    struct timeval now;
    gettimeofday(&now, nullptr);
    double qps = position /
                 (now.tv_sec - start_.tv_sec +
                  1e-6 * (now.tv_usec - start_.tv_usec));
    std::cout << "\033[1K\rTrying " << (100 * position / count) << "%, "
	    << qps/1000 << "Kqps" << std::flush;
  }

  struct timeval start_;
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
