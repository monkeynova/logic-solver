#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

DEFINE_string(benchmarks, "", "...");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  ::benchmark::Initialize(&argc, argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  if (!FLAGS_benchmarks.empty()) {
    ::benchmark::RunSpecifiedBenchmarks();
  }
  return RUN_ALL_TESTS();
}
