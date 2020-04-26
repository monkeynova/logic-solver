#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

DEFINE_bool(benchmark, false,
            "If true, runs benchmarks rather than gunit test suite.");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::testing::InitGoogleTest(&argc, argv);
  ::benchmark::Initialize(&argc, argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  if (FLAGS_benchmark) {
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
  }
  return RUN_ALL_TESTS();
}
