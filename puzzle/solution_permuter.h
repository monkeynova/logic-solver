#ifndef PUZZLE_SOLUTION_PERMUTER_H_
#define PUZZLE_SOLUTION_PERMUTER_H_

#include <memory>

#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

class SolutionPermuter {
 public:
  class AdvancerBase {
   public:
    explicit AdvancerBase(const EntryDescriptor* entry_descriptor);

    virtual ~AdvancerBase() {}

    virtual double position() const = 0;
    virtual double completion() const = 0;
    virtual void Advance() = 0;

    const Solution& current() const { return current_; }

   protected:
    MutableSolution mutable_solution_;
    Solution current_;  // Bound to mutable_solution_.
  };

  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
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
      return advancer_->current() == other.advancer_->current();
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

  SolutionPermuter() = default;
  virtual ~SolutionPermuter() = default;

  virtual bool AddFilter(SolutionFilter solution_filter) { return false; }

  // EntryDescriptor is not valid before this call.
  // TODO(keith@monkeynova.com): Fix this awful interface.
  virtual void Prepare() = 0;

  virtual iterator begin() const = 0;
  virtual iterator end() const = 0;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_PERMUTER_H_
