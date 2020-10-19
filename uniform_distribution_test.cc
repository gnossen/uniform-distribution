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
    std::list<size_t> {1, 2},
  };
  std::set<std::list<size_t>> actual_combinations;
  auto combinations = internal::Combinations(3, 2);
  for (auto combination : combinations) {
    actual_combinations.insert(combination);
  }
  EXPECT_THAT(actual_combinations, ::testing::ContainerEq(expected_combinations));
}

TEST(UniformDistributionTest, CombinationsSunnyDay2) {
  std::set<std::list<size_t>> expected_combinations = {
    std::list<size_t> {0, 1, 2},
    std::list<size_t> {0, 1, 3},
    std::list<size_t> {0, 2, 3},
    std::list<size_t> {1, 2, 3},
  };
  std::set<std::list<size_t>> actual_combinations;
  auto combinations = internal::Combinations(4, 3);
  for (auto combination : combinations) {
    actual_combinations.insert(combination);
  }
  EXPECT_THAT(actual_combinations, ::testing::ContainerEq(expected_combinations));
}

TEST(UniformDistributionTest, CombinationIDontTrustThisThing) {
  for (size_t collection_size = 8; collection_size < 14; ++collection_size) {
    for (size_t selection_size = collection_size - 4; selection_size < collection_size - 1; ++selection_size) {
      std::list<std::list<size_t>> recorded_combinations;
      auto combinations = internal::Combinations(collection_size, selection_size);
      for (auto combination : combinations) {
        size_t last = *combination.begin();
        ASSERT_LT(last, collection_size) << "Value not in proper range";
        for (auto value_iter = ++combination.begin();
            value_iter != combination.end();
            ++value_iter) {
          ASSERT_LT(last, *value_iter) << "Value not sorted";
          ASSERT_LT(*value_iter, collection_size) << "Value not in proper range.";
        }
        ASSERT_EQ(combination.size(), selection_size);
        for (auto recorded_combination : recorded_combinations) {
          EXPECT_THAT(combination,
              ::testing::Not(::testing::ContainerEq(recorded_combination))) << "Found repeat element.";
        }
      }
    }
  }
}

TEST(UniformDistributionTest, Score) {
  std::vector<float> l = {1.0, 2.0, 4.0};
  EXPECT_EQ(score(l), 1.0);
  l = {1.0, 2.0, 3.0, 4.0};
  EXPECT_EQ(score(l), 0.0);
}

std::vector<float> run_exhaustive(const std::vector<float>& input, size_t m) {
  std::vector<float> actual;
  for (auto value : prune_uniform_exhaustive(input, m)) {
    actual.push_back(value);
  }
  return actual;
}

TEST(UniformDistributionTest, ExhaustiveAlgorithm) {
  std::vector<float> input = {1.0, 2.0, 3.0};
  std::vector<float> expected = {1.0, 3.0};
  EXPECT_THAT(run_exhaustive(input, 2), ::testing::ContainerEq(expected));
  input = {1.0, 4.5, 4.6, 5.0, 5.1, 5.2, 6.0, 9.0};
  expected = {1.0, 5.0, 9.0};
  EXPECT_THAT(run_exhaustive(input, 3), ::testing::ContainerEq(expected));
  input = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
  expected = {1.0, 3.0, 5.0, 7.0};
  EXPECT_THAT(run_exhaustive(input, 4), ::testing::ContainerEq(expected));
}

}
