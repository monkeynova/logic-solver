#include "thread/future.h"

#include "absl/synchronization/notification.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "thread/pool.h"

namespace thread {

TEST(FutureTest, Simple) {
  Future<int> test;
  EXPECT_FALSE(test.has_value());
  test.Publish(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(test.WaitForValue(), 123);
}

TEST(FutureTest, Dereference) {
  Future<int> test;
  EXPECT_FALSE(test.has_value());
  test.Publish(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(*test, 123);
}

TEST(FutureTest, Movable) {
  Future<std::unique_ptr<int>> test;
  EXPECT_FALSE(test.has_value());
  test.Publish(std::make_unique<int>(123));
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(*test.WaitForValue(), 123);
  std::unique_ptr<int> recv = std::move(test).WaitForAndConsumeValue();
  EXPECT_EQ(*recv, 123);
}

TEST(FutureTest, Threaded) {
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

TEST(FutureTest, Past) {
  Past<int> test(123);
  EXPECT_TRUE(test.has_value());
  EXPECT_EQ(*test, 123);
}

TEST(FutureTest, FutureSet) {
  FutureSet<int> set;
  Future<int> f1 = set.Create();
  Future<int> f2 = set.Create();
  Future<int>* next = nullptr;
  f2.Publish(123);
  next = set.WaitForAny();
  EXPECT_EQ(next, &f2);
  f1.Publish(456);
  next = set.WaitForAny();
  EXPECT_EQ(next, &f1);
  next = set.WaitForAny();
  EXPECT_EQ(next, nullptr);
}

}  // namespace thread
