#ifndef PUZZLE_FILTER_TO_ACTIVE_SET_H_
#define PUZZLE_FILTER_TO_ACTIVE_SET_H_

#include "absl/base/thread_annotations.h"
#include "absl/functional/function_ref.h"
#include "absl/synchronization/mutex.h"
#include "absl/status/status.h"
#include "puzzle/active_set_pair.h"
#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/profiler.h"
#include "puzzle/solution.h"
#include "puzzle/solution_filter.h"
#include "puzzle/value_skip_to_active_set.h"

namespace puzzle {

class FilterToActiveSet {
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

  FilterToActiveSet(const EntryDescriptor* entry_descriptor,
                    Profiler* profiler = nullptr);

  FilterToActiveSet(const FilterToActiveSet& other);

  const ActiveSet& active_set(int class_int) const {
    DCHECK_LT(static_cast<size_t>(class_int), active_sets_.size());
    // TODO(@monkeynova): This lock guards the hash but not the value
    // which is a terrible contract.
    absl::ReaderMutexLock l(&mu_);
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
  absl::Status Build(const ClassPermuter* class_permuter,
                     const std::vector<SolutionFilter>& predicates);
  absl::Status Build(SingleClassBuild single_class_build,
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
  absl::Status Build(
      const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
      const std::vector<SolutionFilter>& predicates,
      PairClassMode pair_class_mode = PairClassMode::kSingleton) {
    return Build(permuter_a, permuter_b, predicates, predicates,
                 pair_class_mode);
  }
  absl::Status Build(
      PairClassImpl pair_class_impl, const ClassPermuter* permuter_a,
      const ClassPermuter* permuter_b,
      const std::vector<SolutionFilter>& predicates,
      PairClassMode pair_class_mode = PairClassMode::kSingleton) {
    return Build(pair_class_impl, permuter_a, permuter_b, predicates,
                 predicates, pair_class_mode);
  }

  template <PairClassImpl pair_class_impl = PairClassImpl::kBackAndForth>
  absl::Status Build(const ClassPermuter* permuter_a,
                     const ClassPermuter* permuter_b,
                     const std::vector<SolutionFilter>& predicates_by_a,
                     const std::vector<SolutionFilter>& predicates_by_b,
                     PairClassMode pair_class_mode = PairClassMode::kSingleton);
  absl::Status Build(PairClassImpl pair_class_impl,
                     const ClassPermuter* permuter_a,
                     const ClassPermuter* permuter_b,
                     const std::vector<SolutionFilter>& predicates_by_a,
                     const std::vector<SolutionFilter>& predicates_by_b,
                     PairClassMode pair_class_mode = PairClassMode::kSingleton);

 private:
  // Advances `it` based on `value_skip`.
  void Advance(const ValueSkipToActiveSet* vs2as,
               ClassPermuter::iterator::ValueSkip value_skip,
               ClassPermuter::iterator* it) const;

  absl::Status SetupBuild(const ClassPermuter* permuter,
                          const std::vector<SolutionFilter>& predicates);

  absl::Status SetupPairBuild(
      const ClassPermuter* permuter_a, const ClassPermuter* permuter_b,
      const std::vector<SolutionFilter>& predicates_by_a,
      const std::vector<SolutionFilter>& predicates_by_b);

  void SetupPermuter(const ClassPermuter* permuter);

  inline void SingleIterate(
      const ClassPermuter* permuter,
      absl::FunctionRef<bool(const ClassPermuter::iterator& it,
                             ClassPermuter::iterator::ValueSkip* value_skip)>
          on_item);

  inline void DualIterate(
      const ClassPermuter* outer, const ClassPermuter* inner,
      absl::FunctionRef<void(void)> on_outer_before,
      absl::FunctionRef<bool(const ClassPermuter::iterator& it_outer,
                             const ClassPermuter::iterator& it_inner,
                             ClassPermuter::iterator::ValueSkip* inner_skip)>
          on_inner,
      absl::FunctionRef<void(const ClassPermuter::iterator& it_outer,
                             ClassPermuter::iterator::ValueSkip* outer_skip)>
          on_outer_after);

  mutable absl::Mutex mu_;

  // Maps class_int to it's built ActiveSet.
  std::vector<ActiveSet> active_sets_ GUARDED_BY(mu_);

  // active_set_pairs_[class_a][class_b][a_val] stores the ActiveSet for
  // class_b given class_a is at position a_val.
  std::vector<std::vector<ActiveSetPair>> active_set_pairs_;

  absl::flat_hash_map<const Descriptor*, std::unique_ptr<ValueSkipToActiveSet>>
      value_skip_to_active_set_;

  MutableSolution mutable_solution_;
  Solution solution_;  // Bound to mutable_solution_;
  Profiler* profiler_;
};

std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::SingleClassBuild val);
std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::PairClassMode val);
std::ostream& operator<<(std::ostream& out,
                         FilterToActiveSet::PairClassImpl val);

}  // namespace puzzle

#endif  //  PUZZLE_FILTER_TO_ACTIVE_SET_H_
