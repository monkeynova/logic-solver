#include "absl/flags/flag.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gtest/gtest.h"
#include "puzzle/main_lib.h"
#include "puzzle/vlog.h"

ABSL_FLAG(bool, benchmark, false,
          "If true, runs benchmarks rather than gunit test suite.");

int main(int argc, char** argv) {
  std::vector<char*> args = ::puzzle::InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");
  if (absl::GetFlag(FLAGS_benchmark)) {
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
  }
  return RUN_ALL_TESTS();
}
