#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(benchmarks, "", "...");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::benchmark::Initialize(&argc, argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  ::benchmark::RunSpecifiedBenchmarks();
}
