#include "puzzle/active_set.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AnyOf;
using ::testing::Eq;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

namespace puzzle {

TEST(ActiveSetPair, Trivial) {
  EXPECT_TRUE(true);
}

}  // namespace puzzle
