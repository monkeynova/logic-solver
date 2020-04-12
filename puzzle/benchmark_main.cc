#include "benchmark/benchmark.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_bool(benchmark, true,
	    "If false, disables benchmarks. Flag exists to provide compatability with test "
	    "suites that also have the flag.");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::benchmark::Initialize(&argc, argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  if (FLAGS_benchmark) {
    ::benchmark::RunSpecifiedBenchmarks();
  }
  return 0;
}
