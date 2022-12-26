#ifndef EULER_COMPUTER_SCIENCE_VLOG_H
#define EULER_COMPUTER_SCIENCE_VLOG_H

// TODO(@monkeynova): Remove when part of absl/log public API.

namespace internal {

// We use a simple boolean cache of the flag value to avoid any costs
// associated with acquiring the value (like mutex acquisition). The
// value must _never_ be set after flag parsing.
extern int verbosity_level;

}  // namespace internal

#define VLOG_IS_ON(s) (s <= internal::verbosity_level)

#define VLOG(s) LOG_IF(INFO, VLOG_IS_ON(s))
#define VLOG_IF(s, cond) LOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define VLOG_EVERY_N(s, n) LOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define VLOG_IF_EVERY_N(s, cond, n) LOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#define DVLOG(s) DLOG_IF(INFO, VLOG_IS_ON(s))
#define DVLOG_IF(s, cond) DLOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define DVLOG_EVERY_N(s, n) DLOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define DVLOG_IF_EVERY_N(s, cond, n) DLOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#define CHECK_NEAR(f1, f2, epsilon) \
  CHECK(abs((f1) - (f2)) < epsilon) << f1 << " ~ " << f2

#endif  // EULER_COMPUTER_SCIENCE_VLOG_H
