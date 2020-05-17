#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "glog/logging.h"

ABSL_FLAG(bool,benchmark, true,
            "If false, disables benchmarks. Flag exists to provide "
            "compatability with test "
            "suites that also have the flag.");

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::google::InstallFailureSignalHandler();
  ::benchmark::Initialize(&argc, argv);
  std::vector<char*> args = ::absl::ParseCommandLine(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");
  if (absl::GetFlag(FLAGS_benchmark)) {
    ::benchmark::RunSpecifiedBenchmarks();
  }
  return 0;
}
