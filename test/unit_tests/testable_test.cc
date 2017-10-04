#include "testable.h"
#include "gtest/gtest.h"
namespace {

TEST(Testable, ReturnsZero) {
  // Testable = Test case (group of tests)
  // ReturnsZero = Test name
  EXPECT_EQ(returnZero(), 0);
}

}  // namespace
