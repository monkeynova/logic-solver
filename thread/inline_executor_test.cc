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

}  // namespace thread
