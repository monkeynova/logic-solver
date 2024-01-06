#include "absl/flags/flag.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "main_lib.h"
#include "vlog.h"

ABSL_FLAG(bool, benchmark, true,
          "If false, disables benchmarks. Flag exists to provide "
          "compatability with test suites that also have the flag.");

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");
  if (absl::GetFlag(FLAGS_benchmark)) {
    ::benchmark::RunSpecifiedBenchmarks();
  }
  return 0;
}
