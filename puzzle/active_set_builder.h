#ifndef PUZZLE_ACTIVE_SET_BUILDER_H_
#define PUZZLE_ACTIVE_SET_BUILDER_H_

#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"

namespace puzzle {

class ActiveSetBuilder {
 public:
  explicit ActiveSetBuilder(const EntryDescriptor* entry_descriptor) 
    : active_sets_(entry_descriptor == nullptr
		   ? 0 : entry_descriptor->num_classes()),
      mutable_solution_(entry_descriptor) {}

  const ActiveSet& active_set(int class_int) const {
    DCHECK_LT(class_int, active_sets_.size());
    return active_sets_[class_int];
  }
  
  // Given a class permuter and a set of predicates on that class (it is an
  // error to pass in predicates on other classes), returns the ActiveSet
  // representing and ANDing of those predicates on each permutation.
  // The returned active set fully honors the ANDing of input predicates.
  void Build(const ClassPermuter& class_permuter,
	     const std::vector<Solution::Cropper>& predicates);

  // Given a pair of class permuters and a set of predicates on those classes
  // (it is an error to pass predicates on other classes), builds active sets
  // for the permutations such that any value of each permuter that could
  // return a true value with some value of the other permuter will be
  // in the returned active set.
  // The returned active sets are a good faith pruning, but do not fully honor
  // the ANDing of predicates passed in.
  // TODO(keith): Create a first-class concept of a joint ActiveSet.
  void Build(const ClassPermuter& class_permuter_a,
	     const ClassPermuter& class_permuter_b,
	     const std::vector<Solution::Cropper>& predicates);

 private:
  // Maps class_int to it's built ActiveSet.
  std::vector<ActiveSet> active_sets_;
  
  MutableSolution mutable_solution_;
};

}  // namespace puzzle

#endif  //  PUZZLE_ACTIVE_SET_BUILDER_H_
