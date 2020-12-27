// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <initializer_list>
#include <numeric>
#include <sstream>
#include <string>

#include <catch.hpp>
#include <discrete_distribution.hpp>


TEST_CASE("discrete_weights - is constructible from a vector")
{
    std::vector<double> weight_values = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    cxx::discrete_weights weights{weight_values};

    SECTION("attributes are correct")
    {
        auto const expected_sum = std::accumulate(
            weight_values.begin(), weight_values.end(), 0.0
        );
        CHECK(weights.sum() == expected_sum);
        CHECK(weights.weights() == weight_values);
    }
}


TEST_CASE("discrete_weights - is constructible from an initializer list")
{
    std::initializer_list<double> weight_values = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    cxx::discrete_weights weights = weight_values;

    SECTION("attributes are correct")
    {
        auto const expected_sum = std::accumulate(
            weight_values.begin(), weight_values.end(), 0.0
        );
        CHECK(weights.sum() == expected_sum);
        CHECK(weights.weights() == std::vector<double>(weight_values));
    }
}


TEST_CASE("discrete_weights - is equality comparable")
{
    cxx::discrete_weights const weights_A = {1.2, 3.4, 5.6};
    cxx::discrete_weights const weights_B = {1.2, 3.4, 5.6};
    cxx::discrete_weights const weights_C = {5.6, 3.4, 1.2};
    cxx::discrete_weights const weights_D = {1.2, 3.4, 5.6, 7.8};
    cxx::discrete_weights const weights_E = {1.2, 3.4};

    CHECK(weights_A == weights_A);
    CHECK(weights_A == weights_B);
    CHECK(weights_A != weights_C);
    CHECK(weights_A != weights_D);
    CHECK(weights_A != weights_E);
}


TEST_CASE("discrete_weights - is default/copy constructible")
{
    cxx::discrete_weights const origin = {1.0, 2.0, 3.0};
    cxx::discrete_weights const clone = origin;
    cxx::discrete_weights weights;

    weights = origin;

    CHECK(clone == origin);
    CHECK(weights == origin);
}


TEST_CASE("discrete_weights::sum - returns the sum")
{
    cxx::discrete_weights const weights = {1.2, 3.4, 5.6};

    CHECK(weights.sum() == Approx(1.2 + 3.4 + 5.6));
}


TEST_CASE("discrete_weights::weights - returns a vector of weight values")
{
    cxx::discrete_weights const weights = {1.2, 3.4, 5.6};

    std::vector<double> weight_values = weights.weights();
    CHECK(weight_values.size() == 3);
    CHECK(weight_values[0] == 1.2);
    CHECK(weight_values[1] == 3.4);
    CHECK(weight_values[2] == 5.6);
}


TEST_CASE("discrete_weights - is serializable")
{
    cxx::discrete_weights const weights = {1.2, 3.4, 5.6};
    std::string const expected_form = "3 1.2 3.4 5.6";

    std::ostringstream ss;
    ss << weights;

    CHECK(ss.str() == expected_form);
}

TEST_CASE("discrete_weights - is deserializable")
{
    std::string const source_form = "3 1.2 3.4 5.6";
    std::vector<double> const expected_values = {1.2, 3.4, 5.6};

    cxx::discrete_weights weights;
    std::istringstream ss(source_form);
    ss >> weights;

    // Weight values may not exactly match due to numerical errors.
    std::vector<double> const actual_values = weights.weights();

    CHECK(actual_values.size() == expected_values.size());
    CHECK(actual_values[0] == expected_values[0]);
    CHECK(actual_values[1] == expected_values[1]);
    CHECK(actual_values[2] == expected_values[2]);
}
