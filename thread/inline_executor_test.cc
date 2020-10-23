#include "thread/inline_executor.h"

#include "absl/synchronization/notification.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace thread {

TEST(InlineExecutorTest, Trivial) {
  InlineExecutor e;
  bool b = false;
  e.Schedule([&]() { b = true; });
  EXPECT_TRUE(b);
}

TEST(InlineExecutorTest, Future) {
  InlineExecutor e;
  std::unique_ptr<Future<bool>> future =
      e.ScheduleFuture<bool>([]() { return true; });
  EXPECT_TRUE(future->has_value());
  EXPECT_TRUE(future->WaitForValue());
}

}  // namespace thread
