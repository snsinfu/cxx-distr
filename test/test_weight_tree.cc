// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <numeric>
#include <vector>

#include <catch.hpp>
#include <discrete_distribution.hpp>


TEST_CASE("weight_tree - is constructible with given weights")
{
    std::vector<double> const weight_values = {
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0
    };
    cxx::distr_detail::weight_tree weights(weight_values);

    SECTION("attributes are correct")
    {
        CHECK(weights.size() == weight_values.size());

        auto const expected_sum = std::accumulate(
            weight_values.begin(), weight_values.end(), 0.0
        );
        CHECK(weights.sum() == Approx(expected_sum));
    }
}


TEST_CASE("weight_tree - is constructible with empty weights")
{
    std::vector<double> const empty_weights;
    cxx::distr_detail::weight_tree weights(empty_weights);

    SECTION("attributes are correct")
    {
        CHECK(weights.size() == 0);
        CHECK(weights.sum() == 0);
    }
}


TEST_CASE("weight_tree - is equality comparable")
{
    cxx::distr_detail::weight_tree const weights_A({1.2, 3.4, 5.6});
    cxx::distr_detail::weight_tree const weights_B({1.2, 3.4, 5.6});
    cxx::distr_detail::weight_tree const weights_C({5.6, 3.4, 1.2});
    cxx::distr_detail::weight_tree const weights_D({1.2, 3.4, 5.6, 7.8});
    cxx::distr_detail::weight_tree const weights_E({1.2, 3.4});

    CHECK(weights_A == weights_A);
    CHECK(weights_A == weights_B);
    CHECK(weights_A != weights_C);
    CHECK(weights_A != weights_D);
    CHECK(weights_A != weights_E);
}


TEST_CASE("weight_tree - is default constructible")
{
    cxx::distr_detail::weight_tree weights;
    (void) weights;
    // Default state is defined but only assignments are valid.
}


TEST_CASE("weight_tree - is copy constructible")
{
    cxx::distr_detail::weight_tree const origin({1.0, 2.0, 3.0});
    cxx::distr_detail::weight_tree const clone = origin;

    CHECK(clone == origin);
}


TEST_CASE("weight_tree - stores given weight values as-is")
{
    std::vector<double> const weight_values = {
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0
    };
    cxx::distr_detail::weight_tree weights(weight_values);

    auto const size = weights.size();
    auto const data = weights.data();

    for (std::size_t i = 0; i < size; i++) {
        CHECK(data[i] == weight_values[i]);
    }
}


TEST_CASE("weight_tree::update - updates weight value")
{
    cxx::distr_detail::weight_tree weights({1.2, 3.4, 5.6});

    auto const* data = weights.data();
    REQUIRE(data[0] == 1.2);
    REQUIRE(data[1] == 3.4);
    REQUIRE(data[2] == 5.6);
    REQUIRE(weights.sum() == Approx(1.2 + 3.4 + 5.6));

    weights.update(1, 2.3);
    weights.update(2, 3.4);

    // Weight is actually updated in-place.
    CHECK(data[0] == 1.2);
    CHECK(data[1] == 2.3);
    CHECK(data[2] == 3.4);

    // Sum is also updated.
    CHECK(weights.sum() == Approx(1.2 + 2.3 + 3.4));
}


TEST_CASE("weight_tree::find - finds the correct leaf")
{
    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0
    // |----|---------|--------------|
    // |___/|________/|_____________/
    //   0      2            3
    // Note: The element 1 has zero weight, so it won't be found.
    cxx::distr_detail::weight_tree const weights({1.0, 0.0, 2.0, 3.0});

    CHECK(weights.find(0.0) == 0);
    CHECK(weights.find(0.5) == 0);
    CHECK(weights.find(1.0) == 2);
    CHECK(weights.find(1.5) == 2);
    CHECK(weights.find(2.0) == 2);
    CHECK(weights.find(2.5) == 2);
    CHECK(weights.find(3.0) == 3);
    CHECK(weights.find(3.5) == 3);
    CHECK(weights.find(4.0) == 3);
    CHECK(weights.find(4.5) == 3);
    CHECK(weights.find(5.0) == 3);
    CHECK(weights.find(5.5) == 3);
}


TEST_CASE("weight_tree::find - returns edge element for overshoot probe")
{
    // Logically the probe should be in the half-open interval [0, S) where S
    // is the sum of weights. But in practice numerical errors can result in
    // undershoot or overshoot. Here we check robustness against such errors.
    cxx::distr_detail::weight_tree const weights({1.0, 2.0, 3.0});

    CHECK(weights.find(-0.1) == 0);
    CHECK(weights.find(-0.0) == 0);
    CHECK(weights.find(6.0) == 2);
    CHECK(weights.find(6.1) == 2);
}


TEST_CASE("weight_tree::find - finds the correct leaf after weight update")
{
    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0
    // |----|---------|--------------|
    // |___/|________/|_____________/
    //   0      2            3
    cxx::distr_detail::weight_tree weights({1.0, 0.0, 2.0, 3.0});

    REQUIRE(weights.find(2.5) == 2);
    REQUIRE(weights.find(4.0) == 3);
    REQUIRE(weights.find(5.5) == 3);

    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0  7.0  8.0
    // |----|---------|---------|--------------|
    // |___/|________/|________/|_____________/
    //   0      1         2            3
    weights.update(1, 2.0);

    CHECK(weights.find(2.5) == 1);
    CHECK(weights.find(4.0) == 2);
    CHECK(weights.find(5.5) == 3);

    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0
    // |----|---------|--------------|
    // |___/|________/|_____________/
    //   0      1            3
    weights.update(2, 0.0);

    CHECK(weights.find(2.5) == 1);
    CHECK(weights.find(4.0) == 3);
    CHECK(weights.find(5.5) == 3);
}
