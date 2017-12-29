#ifndef PUZZLE_ACTIVE_SET_BUILDER_H_
#define PUZZLE_ACTIVE_SET_BUILDER_H_

#include "puzzle/class_permuter.h"
#include "puzzle/mutable_solution.h"
#include "puzzle/solution.h"

namespace puzzle {

class ActiveSetBuilder {
 public:
  explicit ActiveSetBuilder(const EntryDescriptor* entry_descriptor) 
    : mutable_solution_(entry_descriptor) {}
  
  ActiveSet Build(const ClassPermuter& class_permuter,
		  const std::vector<Solution::Cropper>& predicates);
    
 private:
  MutableSolution mutable_solution_;
};

}  // namespace puzzle

#endif  //  PUZZLE_ACTIVE_SET_BUILDER_H_
