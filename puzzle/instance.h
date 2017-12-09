#ifndef PUZZLE_INSTANCE_H
#define PUZZLE_INSTANCE_H

#include "puzzle/solver.h"

namespace Puzzle {

class PuzzleInstance {
  PuzzleInstance(Puzzle::Solver* solver)
    : solver_(solver) {}
  
  virtual ~PuzzleInstance() {};
  virtual void Setup() = 0;
  virtual void AddProblemPredicates() = 0;
  virtual void AddRulePredicates() = 0;
 protected:
  Puzzle::Solver* solver() { return solver_; }

  // Takes ownership of 'descriptor' and ensures that it outlives 'solver'.
  Puzzle::Descriptor* AddDescriptor(Puzzle::Descriptor* descriptor) {
    descriptors_.emplace_back(descriptor);
    return descriptor;
  }

 private:
  std::vector<std::unique_ptr<Puzzle::Descriptor>> descriptors_;
  Puzzle::Solver* const solver_;
};

}

#endif
