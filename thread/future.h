#ifndef THREAD_FUTURE_H_MONEKYNOVA
#define THREAD_FUTURE_H_MONEKYNOVA

#include "absl/synchronization/notification.h"

namespace thread {

template <typename Storage>
class Future {
 public:
  Future() = default;
  ~Future() { WaitForValue(); }

  bool has_value() const { return publish_.HasBeenNotified(); }

  const Storage& operator*() const { return WaitForValue(); }
  const Storage* operator->() const {
    WaitForValue();
    return &value_;
  }

  void Publish(Storage value) {
    value_ = std::move(value);
    publish_.Notify();
  }

  const Storage& WaitForValue() const {
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

template <typename Storage>
class Past : public Future<Storage> {
 public:
  Past(Storage value) { this->Publish(std::move(value)); }
};

}  // namespace thread

#endif  // THREAD_FUTURE_H_MONEKYNOVA