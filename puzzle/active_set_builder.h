#ifndef PUZZLE_ACTIVE_SET_BUILDER_H_
#define PUZZLE_ACTIVE_SET_BUILDER_H_

#include "puzzle/active_set_pair.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"

namespace puzzle {

class ActiveSetBuilder {
 public:
  enum class SingleClassBuild {
    kPassThrough = 0,
    kPositionSet = 1,
  };
  enum class PairClassMode {
    kSingleton = 0,
    kMakePairs = 1,
  };
  enum class PairClassImpl {
    kPassThroughA = 0,
    kBackAndForth = 1,
    kPairSet = 2,
  };

  explicit ActiveSetBuilder(const EntryDescriptor* entry_descriptor);

  const ActiveSet& active_set(int class_int) const {
    DCHECK_LT(static_cast<size_t>(class_int), active_sets_.size());
    return active_sets_[class_int];
  }
  const ActiveSet& active_set_pair(int class_a, int a_val, int class_b) const {
    DCHECK_NE(class_a, class_b);
    DCHECK_LT(class_a, active_set_pairs_.size());
    DCHECK_LT(class_b, active_set_pairs_[class_a].size());
    return active_set_pairs_[class_a][class_b].Find(a_val);
  }

  // Given a class permuter and a set of predicates on that class (it is an
  // error to pass in predicates on other classes), returns the ActiveSet
  // representing and ANDing of those predicates on each permutation.
  // The returned active set fully honors the ANDing of input predicates.
  template <
      SingleClassBuild single_class_build = SingleClassBuild::kPassThrough>
  void Build(const ClassPermuter* class_permuter,
             const std::vector<SolutionFilter>& predicates);
  void Build(SingleClassBuild single_class_build,
             const ClassPermuter* class_permuter,
             const std::vector<SolutionFilter>& predicates);

  // Given a pair of class permuters and a set of predicates on those classes
  // (it is an error to pass predicates on other classes), builds active sets
  // for the permutations such that any value of each permuter that could
  // return a true value with some value of the other permuter will be
  // in the returned active set.
  // ActiveSets associated with a single permuter are a good faith pruning,
  // but do not fully honor the ANDing of predicates passed in.
  // If pair_class_mode is kMakePairs, the active sets accessible with
  // active_set(class_a, permutation_a, class_b) however is a fully honored
  // active set for the permutations of class_b, given permutation_a.
  // The implementations are built with an eye towards 'permuter_a' being more
  // selective than 'permuter_b', and all other things being equal, ordering
  // the arguments that way may provide a slight performance benefit.
  template <PairClassImpl pair_class_impl = PairClassImpl::kBackAndForth>
  void Build(const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
             const std::vector<SolutionFilter>& predicates,
             PairClassMode pair_class_mode = PairClassMode::kSingleton);
  void Build(PairClassImpl pair_class_impl, const ClassPermuter* permuter_a,
             const ClassPermuter* permuter_b,
             const std::vector<SolutionFilter>& predicates,
             PairClassMode pair_class_mode = PairClassMode::kSingleton);

 private:
  void SetupPairBuild(int class_a, int class_b,
                      const std::vector<SolutionFilter>& predicates) const;

  // Maps class_int to it's built ActiveSet.
  std::vector<ActiveSet> active_sets_;

  // active_set_pairs_[class_a][class_b][a_val] stores the ActiveSet for
  // class_b given class_a is at position a_val.
  std::vector<std::vector<ActiveSetPair>> active_set_pairs_;

  MutableSolution mutable_solution_;
  Solution solution_;  // Bound to mutable_solution_;
};

std::ostream& operator<<(std::ostream& out,
                         ActiveSetBuilder::SingleClassBuild val);
std::ostream& operator<<(std::ostream& out,
                         ActiveSetBuilder::PairClassMode val);
std::ostream& operator<<(std::ostream& out,
                         ActiveSetBuilder::PairClassImpl val);

}  // namespace puzzle

#endif  //  PUZZLE_ACTIVE_SET_BUILDER_H_
