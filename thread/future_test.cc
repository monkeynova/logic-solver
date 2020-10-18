#include "thread/future.h"

#include "absl/synchronization/notification.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "thread/pool.h"

namespace thread {

TEST(ThreadPoolTest, Simple) {
  Future<int> test;
  EXPECT_FALSE(test.has_value());
  test.Publish(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(test.WaitForValue(), 123);
}

TEST(ThreadPoolTest, Dereference) {
  Future<int> test;
  EXPECT_FALSE(test.has_value());
  test.Publish(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(*test, 123);
}

TEST(ThreadPoolTest, Threaded) {
  Future<int> test;
  Pool p(/*num_workers=*/2);
  absl::Notification wait;
  p.Schedule([&]() {
    wait.WaitForNotification();
    test.Publish(123);
  });
  EXPECT_FALSE(test.has_value());
  wait.Notify();
  EXPECT_EQ(test.WaitForValue(), 123);
}

TEST(ThreadPoolTest, Past) {
  Past<int> test(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(*test, 123);
}

}  // namespace thread
