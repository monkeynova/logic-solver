#ifndef THREAD_FUTURE_H_MONEKYNOVA
#define THREAD_FUTURE_H_MONEKYNOVA

#include "absl/synchronization/notification.h"

namespace thread {

template <typename Storage>
class Future {
 public:
  Future() = default;

  bool has_value() const { return publish_.HasBeenNotified(); }

  void Publish(Storage value) {
    value_ = std::move(value);
    publish_.Notify();
  }

  const Storage& WaitForValue() {
    publish_.WaitForNotification();
    return value_;
  }

  Storage WaitForAndConsumeValue() && {
    publish_.WaitForNotification();
    return std::move(value_);
  }

 private:
  Storage value_;
  absl::Notification publish_;
};

}  // namespace thread

#endif  // THREAD_FUTURE_H_MONEKYNOVA