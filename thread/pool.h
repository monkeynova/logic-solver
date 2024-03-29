#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <deque>
#include <thread>
#include <vector>

#include "absl/base/thread_annotations.h"
#include "absl/synchronization/mutex.h"
#include "thread/executor.h"

namespace thread {

// Executor implementation that performs the desired computations within a
// pool of threads allocated to perform the work.
class Pool : public Executor {
 public:
  explicit Pool(int num_threads) {
    pool_.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
      pool_.emplace_back([this]() { Run(); });
    }
  }

  ~Pool() override {
    {
      absl::MutexLock l(&mu_);
      done_ = true;
    }
    for (auto& thread : pool_) {
      if (thread.joinable()) thread.join();
    }
  }

  void Schedule(Fn fn) ABSL_LOCKS_EXCLUDED(mu_) override {
    absl::MutexLock l(&mu_);
    queue_.push_back(fn);
  }

 private:
  bool NoRunningThreads() ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    mu_.AssertHeld();
    return pool_.empty();
  }

  bool QueueNonEmptyOrDone() ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    mu_.AssertHeld();
    return done_ || !queue_.empty();
  }

  void Run() {
    while (true) {
      Fn fn;
      {
        absl::MutexLock l(&mu_);
        mu_.Await(absl::Condition(this, &Pool::QueueNonEmptyOrDone));
        if (queue_.empty() && done_) break;
        fn = std::move(queue_.front());
        queue_.pop_front();
      }
      fn();
    }
  }

  absl::Mutex mu_;
  std::vector<std::thread> pool_;
  std::deque<Fn> queue_ ABSL_GUARDED_BY(mu_);
  bool done_ ABSL_GUARDED_BY(mu_) = false;
};

}  // namespace thread

#endif  // THREAD_POOL_H