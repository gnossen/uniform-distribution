#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <list>
#include <set>
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

TEST(UniformDistributionTest, CombinationsSunnyDay) {
  std::set<std::list<size_t>> expected_combinations = {
    std::list<size_t> {0, 1},
    std::list<size_t> {0, 2},
    std::list<size_t> {1, 0},
    std::list<size_t> {1, 2},
    std::list<size_t> {2, 0},
    std::list<size_t> {2, 1},
  };
  std::set<std::list<size_t>> actual_combinations;
  auto combinations = internal::Combinations(3, 2);
  for (auto combination : combinations) {
    actual_combinations.insert(combination);
  }
  EXPECT_THAT(actual_combinations, ::testing::ContainerEq(expected_combinations));
}

}
