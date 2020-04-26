#include "puzzle/profiler.h"

#include <iomanip>
#include <iostream>
#include <sys/time.h>

#include "absl/memory/memory.h"
#include "gflags/gflags.h"

DEFINE_int32(puzzle_max_profile_calls, std::numeric_limits<int>::max(),
             "Maximum number of iterations before giving up in profiler. "
             "Default value is max_int.");

DEFINE_bool(puzzle_profiler_enable, false,
            "Enables the profiler which tracks time spent performing iterations.");

namespace puzzle {

namespace {

class StructTimevalProfiler : public Profiler {
 public:
  StructTimevalProfiler() : Profiler() {
    gettimeofday(&start_, nullptr);
    last_ = start_;
  }
  ~StructTimevalProfiler() override {
    std::cout << "\033[1K\r" << std::flush;
  }

 private:
  bool Done() override {
    return test_calls() > FLAGS_puzzle_max_profile_calls;
  }

  bool NotePositionImpl(double position, double count) override {
    struct timeval now;
    gettimeofday(&now, nullptr);
    double delta =
         (now.tv_sec - last_.tv_sec + 1e-6 * (now.tv_usec - last_.tv_usec));
    if (delta < 0.2) return false;

    double full_delta =
        (now.tv_sec - start_.tv_sec + 1e-6 * (now.tv_usec - start_.tv_usec));

    double qps = (position - last_position_) / delta;
    std::cout << std::setprecision(3)
              << "\033[1K\rTrying " << (100 * position / count)
              << "%, effective=" << qps/1000 << "Kqps true="
              << (test_calls() / full_delta / 1000) << "Kqps"
              << std::flush;
    last_ = now;
    last_position_ = position;
    return true;
  }

  void NoteFinish() override {
    gettimeofday(&last_, nullptr);
  }

  double Seconds() override {
    return
      last_.tv_sec - start_.tv_sec + 1e-6 * (last_.tv_usec - start_.tv_usec);
  }

  struct timeval start_;
  struct timeval last_;
  double last_position_;
};

}  // namespace

std::unique_ptr<Profiler> Profiler::Create() {
  if (!FLAGS_puzzle_profiler_enable) {
    return nullptr;
  }

  return absl::make_unique<StructTimevalProfiler>();
}

}  // namespace puzzle
