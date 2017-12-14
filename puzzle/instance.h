#ifndef PUZZLE_INSTANCE_H
#define PUZZLE_INSTANCE_H

#include "puzzle/solver.h"

namespace puzzle {

class PuzzleInstance {
  PuzzleInstance(puzzle::Solver* solver)
    : solver_(solver) {}
  
  virtual ~PuzzleInstance() {};

  // Create the
  virtual void Setup() = 0;
  virtual void AddProblemPredicates() = 0;
  virtual void AddRulePredicates() = 0;
  
 protected:
  puzzle::Solver* solver() { return solver_; }

  // Takes ownership of 'descriptor' and ensures that it outlives 'solver'.
  puzzle::Descriptor* AddDescriptor(puzzle::Descriptor* descriptor) {
    descriptors_.emplace_back(descriptor);
    return descriptor;
  }

 private:
  std::vector<std::unique_ptr<puzzle::Descriptor>> descriptors_;
  puzzle::Solver* const solver_;
};

}

#endif
