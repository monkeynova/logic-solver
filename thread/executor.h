#ifndef THREAD_EXECUTOR_H
#define THREAD_EXECUTOR_H

#include <functional>

namespace thread {

class Executor {
 public:
  using Fn = std::function<void()>;

  virtual void Schedule(Fn fn) = 0;
};

}  // namespace thread

#endif  // THREAD_EXECUTOR_H