#ifndef THREAD_EXECUTOR_H
#define THREAD_EXECUTOR_H

#include <functional>

namespace thread {

// Abstract base class to peform possibly delayed, queued or concurrent
// calculation of arbitrary work units.
class Executor {
 public:
  virtual ~Executor() = default;

  using Fn = std::function<void()>;

  virtual void Schedule(Fn fn) = 0;
};

}  // namespace thread

#endif  // THREAD_EXECUTOR_H