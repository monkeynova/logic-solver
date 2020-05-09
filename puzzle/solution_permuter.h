#ifndef PUZZLE_SOLUTION_PERMUTER_H_
#define PUZZLE_SOLUTION_PERMUTER_H_

#include <memory>

#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

class SolutionPermuter {
 public:
  class AdvanceInterface {
   public:
    virtual ~AdvanceInterface() {}

    virtual double position() const = 0;
    virtual double completion() const = 0;
    virtual const Solution& current() const = 0;
    virtual void Advance() = 0;
  };

  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    typedef Solution value_type;
    typedef Solution& reference;
    typedef Solution* pointer;

    explicit iterator(std::unique_ptr<AdvanceInterface> advancer)
        : advancer_(std::move(advancer)) {}

    iterator(const iterator&) = delete;
    iterator(iterator&&) = default;
    iterator& operator=(const iterator&) = delete;
    iterator& operator=(iterator&&) = default;

    bool operator!=(const iterator& other) { return !(*this == other); }
    bool operator==(const iterator& other) {
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
    std::unique_ptr<AdvanceInterface> advancer_;
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
