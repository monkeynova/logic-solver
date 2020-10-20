#ifndef THREAD_FUTURE_H_MONEKYNOVA
#define THREAD_FUTURE_H_MONEKYNOVA

#include <queue>

#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"

namespace thread {

template <typename Storage> class FutureSet;

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
    if (future_set_ != nullptr) {
      future_set_->Notify(this);
    }
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
  explicit Future(FutureSet<Storage>* future_set)
   : future_set_(future_set) {}

  Storage value_;
  absl::Notification publish_;
  FutureSet<Storage>* future_set_ = nullptr;

  friend class FutureSet<Storage>;
};

template <typename Storage>
class FutureSet {
 public:
  FutureSet() = default;

  Future<Storage> Create() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    ++outstanding_futures_;
    return Future<Storage>(this);
  }

  Future<Storage>* WaitForAny() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    mu_.Await(absl::Condition(this, &FutureSet::AnyReady));
    if (queue_.empty()) return nullptr;
    Future<Storage>* next = queue_.front();
    queue_.pop();
    return next;
  }

  void Notify(Future<Storage>* ready) ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    --outstanding_futures_;
    queue_.push(ready);
  }

 private:
  bool AnyReady() const ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    return outstanding_futures_ == 0 || !queue_.empty();
  }

  absl::Mutex mu_;
  int outstanding_futures_ GUARDED_BY(mu_) = 0;
  std::queue<Future<Storage>*> queue_ GUARDED_BY(mu_);
};

template <typename Storage>
class Past : public Future<Storage> {
 public:
  Past(Storage value) { this->Publish(std::move(value)); }
};

}  // namespace thread

#endif  // THREAD_FUTURE_H_MONEKYNOVA