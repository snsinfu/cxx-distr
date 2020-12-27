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


TEST_CASE("discrete_distribution - is default constructible")
{
    cxx::discrete_distribution distr;
    (void) distr;
}


TEST_CASE("discrete_distribution - is constructible from weights")
{
    // Vector
    std::vector<double> const values = {1.0, 2.0, 3.0};
    cxx::discrete_distribution distr_v{values};
    (void) distr_v;

    // Initializer list
    cxx::discrete_distribution distr_i = {1.0, 2.0, 3.0};
    (void) distr_i;

    // Weights
    cxx::discrete_weights weights = {1.0, 2.0, 3.0};
    cxx::discrete_distribution distr_w{weights};
    (void) distr_w;
}


TEST_CASE("discrete_distribution - is equality comparable")
{
    cxx::discrete_distribution const distr_A = {1.2, 3.4, 5.6};
    cxx::discrete_distribution const distr_B = {1.2, 3.4, 5.6};
    cxx::discrete_distribution const distr_C = {5.6, 3.4, 1.2};
    cxx::discrete_distribution const distr_D = {1.2, 3.4, 5.6, 7.8};
    cxx::discrete_distribution const distr_E = {1.2, 3.4};

    CHECK(distr_A == distr_A);
    CHECK(distr_A == distr_B);
    CHECK(distr_A != distr_C);
    CHECK(distr_A != distr_D);
    CHECK(distr_A != distr_E);
}


TEST_CASE("discrete_distribution - is copyable")
{
    cxx::discrete_distribution origin = {1.0, 2.0, 3.0};
    cxx::discrete_distribution clone = origin;
    cxx::discrete_distribution distr;

    distr = origin;

    CHECK(clone == origin);
    CHECK(distr == origin);
}


TEST_CASE("discrete_distribution::reset - is defined")
{
    // Just a RandomNumberDistribution requirement.
    cxx::discrete_distribution distr;
    distr.reset();
}


TEST_CASE("discrete_distribution::param - roundtrip works")
{
    cxx::discrete_distribution const origin = {1.2, 3.4, 5.6};
    cxx::discrete_distribution distr;

    distr.param(origin.param());

    CHECK(distr == origin);
}


TEST_CASE("discrete_distribution::param - is-a discrete_weights")
{
    cxx::discrete_weights const expected_weights = {1.2, 3.4, 5.6};
    cxx::discrete_distribution const distr{expected_weights};

    cxx::discrete_weights const& weights = distr.param();
    CHECK(weights == expected_weights);
}


TEST_CASE("discrete_distribution::min/max - returns correct bounds")
{
    cxx::discrete_distribution const distr1 = {1.0};
    CHECK(distr1.min() == 0);
    CHECK(distr1.max() == 0);

    cxx::discrete_distribution const distr2 = {1.0, 2.0};
    CHECK(distr2.min() == 0);
    CHECK(distr2.max() == 1);

    cxx::discrete_distribution const distr3 = {1.0, 2.0, 3.0};
    CHECK(distr3.min() == 0);
    CHECK(distr3.max() == 2);
}


TEST_CASE("discrete_distribution::sum - returns correct weight sum")
{
    cxx::discrete_distribution const distr = {1.2, 3.4, 5.6};
    CHECK(distr.sum() == Approx(1.2 + 3.4 + 5.6));
}


TEST_CASE("discrete_distribution::update - updates weight value")
{
    cxx::discrete_distribution distr = {1.2, 3.4, 5.6};

    auto const& weights = distr.param();
    REQUIRE(weights[0] == 1.2);
    REQUIRE(weights[1] == 3.4);
    REQUIRE(weights[2] == 5.6);

    distr.update(1, 2.3);
    distr.update(2, 3.4);

    CHECK(weights[0] == 1.2);
    CHECK(weights[1] == 2.3);
    CHECK(weights[2] == 3.4);
}


TEST_CASE("discrete_distribution - generates values in correct probability")
{
    // Define event weights. Note the second event with zero weight.
    std::vector<double> const weights = {1.0, 0.0, 2.0, 3.0, 4.0};
    auto const weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0);

    // Sample from the discrete distribution and construct an empirical
    // distribution (histogram).
    cxx::discrete_distribution distr{weights};

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

    // The distribution must not generate a value with zero weight. This
    // property must be exact, not approximate.
    CHECK(histogram[1] == 0);
}


TEST_CASE("discrete_distribution - changes distribution on the fly")
{
    cxx::discrete_distribution distr = {1.0, 0.0};
    std::mt19937_64 random;

    // Now the distribution is {1, 0}, so only the first event occurs.
    CHECK(distr(random) == 0);
    CHECK(distr(random) == 0);
    CHECK(distr(random) == 0);

    // Change the distribution to {0, 1}.
    distr.update(0, 0.0);
    distr.update(1, 1.0);

    CHECK(distr(random) == 1);
    CHECK(distr(random) == 1);
    CHECK(distr(random) == 1);
}
