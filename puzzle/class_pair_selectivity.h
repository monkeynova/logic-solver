#ifndef PUZZLE_CLASS_PAIR_SELECTIVITY_H
#define PUZZLE_CLASS_PAIR_SELECTIVITY_H

#include <vector>

#include "puzzle/class_permuter.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

// Stores the joint selectivity of a pair of `ClassPermuters` which can then
// be used to prioritized pruning efforts between pairs of permuters.
class ClassPairSelectivity {
 public:
  ClassPairSelectivity(ClassPermuter* a, ClassPermuter* b,
                       const std::vector<SolutionFilter>* filters_by_a,
                       const std::vector<SolutionFilter>* filters_by_b)
      : a_(a),
        b_(b),
        filters_by_a_(filters_by_a),
        filters_by_b_(filters_by_b),
        computed_a_(false),
        computed_b_(false) {}
  ClassPermuter* a() const { return a_; }
  ClassPermuter* b() const { return b_; }
  const std::vector<SolutionFilter>* filters_by_a() const {
    return filters_by_a_;
  }
  const std::vector<SolutionFilter>* filters_by_b() const {
    return filters_by_b_;
  }
  double pair_selectivity() const { return pair_selectivity_; }
  // TODO(@monkeynova): In theory computed should be the AND-ing of the two
  // parts. In practice, it causes a 15% regression in benchmarks on sudoku
  // since it redoes a bunch of redundant work. This method probably should
  // be reamed at the least, but understanding the model for when to compute
  // further would be better.
  bool computed() const { return computed_a_ || computed_b_; }
  void set_computed_a(bool computed) { computed_a_ = computed; }
  void set_computed_b(bool computed) { computed_b_ = computed; }
  void SetPairSelectivity(const FilterToActiveSet* filter_to_active_set) {
    double a_selectivity =
        filter_to_active_set->active_set(a_->class_int()).Selectivity();
    double b_selectivity =
        filter_to_active_set->active_set(b_->class_int()).Selectivity();
    if (a_selectivity > b_selectivity) {
      // Make `a` less selective than `b` for Build calls.
      // TODO(@monkeynova): Leave not-computed on "the right" and pass down
      // that information to FilterToActiveSet.
      std::swap(a_, b_);
      std::swap(filters_by_a_, filters_by_b_);
      std::swap(computed_a_, computed_b_);
    }
    pair_selectivity_ = a_selectivity * b_selectivity;
  }
  // TODO(@monkeynova): This metric currently is based on the worst-case
  // cost of computing the pair-wise active sets (N^2 cost). But this is
  // neither the expected cost of the computation (early exit), nor is
  // that even the ideal metric which is the ROI on future compute reduction.
  bool operator<(const ClassPairSelectivity& other) const {
    if (computed() ^ other.computed()) {
      // Computed is "greater than" non-computed.
      return other.computed();
    }
    return pair_selectivity() < other.pair_selectivity();
  }

 private:
  ClassPermuter* a_;
  ClassPermuter* b_;
  const std::vector<SolutionFilter>* filters_by_a_;
  const std::vector<SolutionFilter>* filters_by_b_;
  double pair_selectivity_;
  bool computed_a_;
  bool computed_b_;
};

// Greater than comparison for ClassPairSelectivity for use in a heap.
struct ClassPairSelectivityGreaterThan {
  bool operator()(const ClassPairSelectivity& a,
                  const ClassPairSelectivity& b) const {
    return b < a;
  }
};

}  // namespace puzzle
#endif  // PUZZLE_CLASS_PAIR_SELECTIVITY_H