#include "thread/pool.h"

#include "absl/synchronization/notification.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace thread {

TEST(ThreadPoolTest, Trivial) {
  bool set = false;
  {
    Pool p(/*num_workers=*/1);
    p.Schedule([&set]() { set = true; });
  }
  EXPECT_TRUE(set); 
}

TEST(ThreadPoolTest, ConcurrentWork) {
  absl::Mutex mu;
  int sum = 0;
  bool done = false;
  const int kNonWaitingThreads = 10;
  absl::Notification wait;
  {
    Pool p(/*num_workers=*/2);
    p.Schedule([&]() {
        wait.WaitForNotification();
        absl::MutexLock l(&mu);
        ++sum;
    });
    for (int i = 0; i < kNonWaitingThreads; ++i) {
      p.Schedule([&]() {
          absl::MutexLock l(&mu);
          ++sum;
          if (sum == kNonWaitingThreads) done = true;
      });
    }
    absl::MutexLock l(&mu);
    ASSERT_TRUE(mu.AwaitWithTimeout(absl::Condition(&done), absl::Seconds(1)));
    wait.Notify();
  }
  EXPECT_EQ(sum, kNonWaitingThreads + 1);
}


}  // namespace puzzle
