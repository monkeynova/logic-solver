#include "puzzle/profiler.h"

#include <iomanip>
#include <iostream>

#include "absl/flags/flag.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/memory/memory.h"
#include "absl/time/time.h"
#include "puzzle/vlog.h"

ABSL_FLAG(int, puzzle_max_profile_calls, std::numeric_limits<int>::max(),
          "Maximum number of iterations before giving up in profiler. "
          "Default value is max_int.");

ABSL_FLAG(
    bool, puzzle_profiler_enable, false,
    "Enables the profiler which tracks time spent performing iterations.");

namespace puzzle {

namespace {

class AbslTimeProfiler : public Profiler {
 public:
  AbslTimeProfiler() : Profiler() {
    start_ = absl::Now();
    last_permutation_ = start_;
    last_prepare_ = start_;
  }
  ~AbslTimeProfiler() override { std::cout << "\033[1K\r" << std::flush; }

 private:
  bool Done() override {
    return permutations() > absl::GetFlag(FLAGS_puzzle_max_profile_calls);
  }

  bool NotePermutationImpl(int64_t position, int64_t count) override {
    absl::Time now = absl::Now();
    int delta = (now - last_permutation_) / absl::Microseconds(1);
    if (delta < 200) return false;

    int full_delta = (now - start_) / absl::Microseconds(1);

    double permutations_per_milli = (position - last_position_) / delta;
    std::cout << std::setprecision(3) << "\033[1K\rTrying "
              << (100 * position / count)
              << "%, effective=" << permutations_per_milli
              << "Kp/ms true=" << (permutations() / full_delta) << "Kp/ms"
              << std::flush;
    last_permutation_ = now;
    last_position_ = position;
    return true;
  }

  void NotePrepareImpl(int64_t position, int64_t count) override {
    absl::Time now = absl::Now();
    int delta = (now - last_prepare_) / absl::Milliseconds(1);
    if (delta < 1000) return;

    int full_delta = (now - start_) / absl::Milliseconds(1);
    LOG(INFO) << "Prepare: " << position << "/" << count << " ("
              << 100 * position / count
              << "%); Rate=" << prepare_steps() / full_delta << "Kqps";
    last_prepare_ = now;
  }

  void NoteFinish() override { last_permutation_ = absl::Now(); }

  double Seconds() override {
    return 1e-3 * ((last_permutation_ - start_) / absl::Milliseconds(1));
  }

  absl::Time start_;
  absl::Time last_permutation_;
  absl::Time last_prepare_;
  double last_position_;
};

}  // namespace

std::unique_ptr<Profiler> Profiler::Create() {
  if (!absl::GetFlag(FLAGS_puzzle_profiler_enable)) {
    return nullptr;
  }

  return absl::make_unique<AbslTimeProfiler>();
}

}  // namespace puzzle
