#ifndef PUZZLE_SOLUTION_PERMUTER_H_
#define PUZZLE_SOLUTION_PERMUTER_H_

#include <memory>

#include "absl/status/statusor.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

class SolutionPermuter {
 public:
  class AdvancerBase {
   public:
    explicit AdvancerBase(const EntryDescriptor* entry_descriptor);

    virtual ~AdvancerBase() = default;

    bool done() const { return done_; }
    void set_done() { done_ = true; }

    virtual double position() const = 0;
    virtual double completion() const = 0;
    virtual void Advance() = 0;

    const Solution& current() const { return current_; }

   protected:
    MutableSolution mutable_solution_;
    Solution current_;  // Bound to mutable_solution_.
    bool done_ = false;
  };

  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef double difference_type;
    typedef Solution value_type;
    typedef Solution& reference;
    typedef Solution* pointer;

    explicit iterator(std::unique_ptr<AdvancerBase> advancer)
        : advancer_(std::move(advancer)) {}

    iterator(const iterator&) = delete;
    iterator(iterator&&) = default;
    iterator& operator=(const iterator&) = delete;
    iterator& operator=(iterator&&) = default;

    bool operator!=(const iterator& other) { return !(*this == other); }
    bool operator==(const iterator& other) {
      if (advancer_ == nullptr) return other.advancer_ == nullptr;
      if (other.advancer_ == nullptr) return false;
      return advancer_->done() == other.advancer_->done();
    }
    const Solution& operator*() { return advancer_->current(); }
    const Solution* operator->() { return &advancer_->current(); }
    iterator& operator++() {
      advancer_->Advance();
      return *this;
    }

    double position() const { return advancer_->position(); }
    double completion() const { return advancer_->completion(); }

   private:
    std::unique_ptr<AdvancerBase> advancer_;
  };

  explicit SolutionPermuter(const EntryDescriptor* entry_descriptor)
      : entry_descriptor_(entry_descriptor) {}
  virtual ~SolutionPermuter() = default;

  virtual absl::StatusOr<bool> AddFilter(SolutionFilter solution_filter) {
    return false;
  }

  virtual absl::Status Prepare() = 0;

  virtual iterator begin() const = 0;
  virtual iterator end() const = 0;

 protected:
  const EntryDescriptor* entry_descriptor() const { return entry_descriptor_; }

 private:
  const EntryDescriptor* entry_descriptor_;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_PERMUTER_H_
