#ifndef __PUZZLE_BRUTE_SOLUTION_PERMUTER_H
#define __PUZZLE_BRUTE_SOLUTION_PERMUTER_H

#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"
#include "puzzle/solution_permuter.h"

namespace puzzle {

class BruteSolutionPermuter final : public SolutionPermuter {
 public:
  class Advancer final : public SolutionPermuter::AdvanceInterface {
   public:
    explicit Advancer(const BruteSolutionPermuter* permuter,
		      const EntryDescriptor* entry_descriptor);

    Advancer(const Advancer&) = delete;
    Advancer& operator=(const Advancer&) = delete;

    double position() const override;
    double completion() const override;

   private:
    const Solution& current() const override { return current_; }
    void Advance() override;

    const BruteSolutionPermuter* permuter_;
    MutableSolution mutable_solution_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
    Solution current_;  // Bound to mutable_solution_.
  };

  explicit BruteSolutionPermuter(const EntryDescriptor* e);
  ~BruteSolutionPermuter() = default;

  // Movable, but not copyable.
  BruteSolutionPermuter(const BruteSolutionPermuter&) = delete;
  BruteSolutionPermuter& operator=(const BruteSolutionPermuter&) = delete;
  BruteSolutionPermuter(BruteSolutionPermuter&&) = default;
  BruteSolutionPermuter& operator=(BruteSolutionPermuter&&) = default;

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, entry_descriptor_));
  }
  iterator end() const override {
    return iterator(absl::make_unique<Advancer>(this, nullptr));
  }

  double permutation_count() const;
  const ClassPermuter& class_permuter(int class_int) const {
    return class_permuters_[class_int];
  }

 private:
  const EntryDescriptor* entry_descriptor_;
  std::vector<ClassPermuter> class_permuters_;

  friend Advancer;
};

}  // namespace puzzle

#endif  // __PUZZLE_BRUTE_SOLUTION_PERMUTER_H
