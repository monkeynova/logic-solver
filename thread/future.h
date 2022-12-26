#ifndef THREAD_FUTURE_H_MONEKYNOVA
#define THREAD_FUTURE_H_MONEKYNOVA

#include <memory>
#include <queue>

#include "absl/base/thread_annotations.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "puzzle/vlog.h"

namespace thread {

template <typename Storage>
class FutureSet;

template <typename Storage>
class Future {
 public:
  Future() = default;
  ~Future() { WaitForValue(); }

  bool has_value() const {
    absl::MutexLock l(&mu_);
    return has_value_locked();
  }

  const Storage& operator*() const { return WaitForValue(); }
  const Storage* operator->() const { return &WaitForValue(); }

  void Publish(Storage value) {
    {
      absl::MutexLock l(&mu_);
      DCHECK(!has_value_);
      value_ = std::move(value);
      has_value_ = true;
    }
    if (future_set_ != nullptr) {
      future_set_->Notify(this);
    }
  }

  const Storage& WaitForValue() const {
    absl::MutexLock l(&mu_);
    mu_.Await(absl::Condition(this, &Future::has_value_locked));
    return value_;
  }

  Storage WaitForAndConsumeValue() && {
    absl::MutexLock l(&mu_);
    mu_.Await(absl::Condition(this, &Future::has_value_locked));
    return std::move(value_);
  }

 private:
  bool has_value_locked() const ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    mu_.AssertHeld();
    return has_value_;
  }

  explicit Future(FutureSet<Storage>* future_set) : future_set_(future_set) {}

  mutable absl::Mutex mu_;
  bool has_value_ ABSL_GUARDED_BY(mu_) = false;
  Storage value_ ABSL_GUARDED_BY(mu_);
  FutureSet<Storage>* future_set_ = nullptr;

  friend class FutureSet<Storage>;
};

template <typename Storage>
class FutureSet {
 public:
  FutureSet() = default;

  Future<Storage>* Create() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    ++outstanding_futures_;
    futures_.emplace_back(new Future<Storage>(this));
    return futures_.back().get();
  }

  Future<Storage>* WaitForAny() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    mu_.Await(absl::Condition(this, &FutureSet::AnyReady));
    if (ready_.empty()) return nullptr;
    Future<Storage>* next = ready_.front();
    ready_.pop();
    return next;
  }

  void Notify(Future<Storage>* ready) ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock l(&mu_);
    --outstanding_futures_;
    ready_.push(ready);
  }

 private:
  bool AnyReady() const ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    return outstanding_futures_ == 0 || !ready_.empty();
  }

  absl::Mutex mu_;
  int outstanding_futures_ ABSL_GUARDED_BY(mu_) = 0;
  std::vector<std::unique_ptr<Future<Storage>>> futures_ ABSL_GUARDED_BY(mu_);
  std::queue<Future<Storage>*> ready_ ABSL_GUARDED_BY(mu_);
};

template <typename Storage>
class Past : public Future<Storage> {
 public:
  Past(Storage value) { this->Publish(std::move(value)); }
};

}  // namespace thread

#endif  // THREAD_FUTURE_H_MONEKYNOVA
