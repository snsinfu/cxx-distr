// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <initializer_list>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <catch.hpp>
#include <discrete_distribution.hpp>


TEST_CASE("discrete_distribution - generates values in correct probability")
{
    std::vector<double> const weights = {1.0, 0.0, 2.0, 3.0, 4.0};
    auto const weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0);

    cxx::discrete_distribution distr(weights);

    int const sample_count = 10000;
    std::mt19937_64 random;
    std::vector<double> histogram(distr.max() + 1);

    double const sample_weight = weight_sum / double(sample_count);
    for (int sample = 0; sample < sample_count; sample++) {
        histogram[distr(random)] += sample_weight;
    }

    // Check empirical distribution.
    for (std::size_t i = distr.min(); i <= distr.max(); i++) {
        CHECK(histogram[i] == Approx(weights[i]).epsilon(0.1));
    }

    // The distribution must not generate a value with zero weight.
    CHECK(histogram[1] == 0);
}
