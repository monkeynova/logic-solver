#include "puzzle/brute_solution_permuter.h"

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(BruteSolutionPermuter, Simple) {
  Puzzle::EntryDescriptor ed;
  Puzzle::IntRangeDescriptor id(3, 5);
  Puzzle::IntRangeDescriptor cd1(6, 8);
  Puzzle::IntRangeDescriptor cd2(11, 13);
  
  ed.SetIds(&id);
  ed.SetClass(0,"foo",&cd1);
  ed.SetClass(1,"bar",&cd2);

  Puzzle::BruteSolutionPermuter p(&ed, {});
  EXPECT_THAT(p.permutation_count(), 6 * 6);
  int position = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    EXPECT_THAT(it.position(), position);
    std::cout << it.position() << "/" << p.permutation_count() << "=="
	      << it.completion() << std::endl;
    std::cout << it->ToStr() << std::endl;
    position++;
  }
  EXPECT_THAT(position, 6 * 6);
}
