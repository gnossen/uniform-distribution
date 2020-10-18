#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <list>
#include <vector>

#include "uniform_distribution.h"

namespace {

using namespace gnossen::uniform_distribution;

TEST(UniformDistributionTest, OutputIteratorSunnyDay) {
  std::vector<unsigned int> base_container = {1, 2, 3, 4, 5, 6};
  std::list<size_t> to_remove = {0, 2, 4};
  std::vector<unsigned int> expected_output = {2, 4, 6};
  OutputContainer<decltype(base_container)> output_container(&base_container, to_remove);
  std::vector<unsigned int> actual_output;
  for (unsigned int value : output_container) {
    actual_output.push_back(value);
  }
  EXPECT_THAT(actual_output, ::testing::ContainerEq(expected_output));
}

}
