#ifndef THREAD_EXECUTOR_H
#define THREAD_EXECUTOR_H

#include <functional>

#include "thread/future.h"

namespace thread {

// Abstract base class to peform possibly delayed, queued or concurrent
// calculation of arbitrary work units.
class Executor {
 public:
  virtual ~Executor() = default;

  using Fn = std::function<void()>;

  virtual void Schedule(Fn fn) = 0;

  template <typename FnType,
            typename Storage = typename std::invoke_result_t<FnType>>
  std::unique_ptr<Future<Storage>> ScheduleFuture(FnType fn) {
    std::unique_ptr<Future<Storage>> ret(new Future<Storage>());
    Future<Storage>* raw_ret = ret.get();
    Schedule([raw_ret, fn]() { raw_ret->Publish(fn()); });
    return ret;
  }

  template <typename FnType,
            typename Storage = typename std::invoke_result_t<FnType>>
  Future<Storage>* ScheduleFuture(FutureSet<Storage>* set, FnType fn) {
    Future<Storage>* ret = set->Create();
    Schedule([ret, fn]() { ret->Publish(fn()); });
    return ret;
  }
};

}  // namespace thread

#endif  // THREAD_EXECUTOR_H