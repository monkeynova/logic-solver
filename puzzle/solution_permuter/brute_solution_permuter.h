#ifndef PUZZLE_SOLUTION_PERMUTER_BRUTE_SOLUTION_PERMUTER_H
#define PUZZLE_SOLUTION_PERMUTER_BRUTE_SOLUTION_PERMUTER_H

#include "puzzle/base/solution.h"
#include "puzzle/class_permuter/class_permuter.h"
#include "puzzle/solution_permuter/solution_permuter.h"

namespace puzzle {

class BruteSolutionPermuter final : public SolutionPermuter {
 public:
  class Advancer final : public SolutionPermuter::AdvancerBase {
   public:
    explicit Advancer(const BruteSolutionPermuter* permuter,
                      const EntryDescriptor* entry_descriptor);

    Advancer(const Advancer&) = delete;
    Advancer& operator=(const Advancer&) = delete;

    Position position() const;

   private:
    void Advance() override;

    const BruteSolutionPermuter* permuter_;
    std::vector<int> class_types_;
    std::vector<ClassPermuter::iterator> iterators_;
  };

  explicit BruteSolutionPermuter(const EntryDescriptor* e);
  ~BruteSolutionPermuter() = default;

  // Movable, but not copyable.
  BruteSolutionPermuter(const BruteSolutionPermuter&) = delete;
  BruteSolutionPermuter& operator=(const BruteSolutionPermuter&) = delete;
  BruteSolutionPermuter(BruteSolutionPermuter&&) = default;
  BruteSolutionPermuter& operator=(BruteSolutionPermuter&&) = default;

  absl::Status PrepareCheap() override;
  absl::Status PrepareFull() override;

  absl::StatusOr<bool> AddFilter(SolutionFilter solution_filter) override {
    return false;
  }

  double Selectivity() const override { return 1.0; }

  iterator begin() const override {
    return iterator(absl::make_unique<Advancer>(this, entry_descriptor()));
  }

  double permutation_count() const;
  const ClassPermuter* class_permuter(int class_int) const {
    return class_permuters_[class_int].get();
  }

 private:
  std::vector<std::unique_ptr<ClassPermuter>> class_permuters_;

  friend Advancer;
};

}  // namespace puzzle

#endif  // PUZZLE_SOLUTION_PERMUTER_BRUTE_SOLUTION_PERMUTER_H
