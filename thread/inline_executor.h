#ifndef THREAD_INLINE_EXECUTOR_H
#define THREAD_INLINE_EXECUTOR_H

#include "thread/executor.h"

namespace thread {

class InlineExecutor : public Executor {
 public:
  void Schedule(Fn fn) override { fn(); }
};

}  // namespace thread

#endif  // THREAD_INLINE_EXECUTOR_H