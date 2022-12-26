#include "puzzle/vlog.h"

#include "absl/flags/flag.h"

namespace internal {

int verbosity_level = 0;

}  // namespace internal

ABSL_FLAG(int, v, 0, "The verbosity level for VLOG.").OnUpdate([] {
  internal::verbosity_level = absl::GetFlag(FLAGS_v);
});
