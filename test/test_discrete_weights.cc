// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <initializer_list>
#include <numeric>
#include <sstream>
#include <string>

#include <catch.hpp>
#include <discrete_distribution.hpp>


TEST_CASE("discrete_weights - is default constructible")
{
    cxx::discrete_weights weights;
    (void) weights;
}


TEST_CASE("discrete_weights - is constructible from weights")
{
    // Vector
    std::vector<double> const values = {1.0, 2.0, 3.0};
    cxx::discrete_weights weights_v{values};
    (void) weights_v;

    // Initializer list
    cxx::discrete_weights weights_i = {1.0, 2.0, 3.0};
    (void) weights_i;
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


TEST_CASE("discrete_weights - is copyable")
{
    cxx::discrete_weights origin = {1.0, 2.0, 3.0};
    cxx::discrete_weights clone = origin;
    cxx::discrete_weights weights;

    weights = origin;

    CHECK(clone == origin);
    CHECK(weights == origin);
}


TEST_CASE("discrete_weights::size - is the number of events")
{
    cxx::discrete_weights weights1 = {1.0};
    cxx::discrete_weights weights2 = {1.0, 2.0};
    cxx::discrete_weights weights3 = {1.0, 2.0, 3.0};

    CHECK(weights1.size() == 1);
    CHECK(weights2.size() == 2);
    CHECK(weights3.size() == 3);
}


TEST_CASE("discrete_weights::data - points to the weight values")
{
    cxx::discrete_weights const weights = {1.0, 2.0, 3.0};

    double const* values = weights.data();
    CHECK(values[0] == 1.0);
    CHECK(values[1] == 2.0);
    CHECK(values[2] == 3.0);
}


TEST_CASE("discrete_weights::begin/end - contains the weight values")
{
    std::vector<double> const expected = {1.0, 2.0, 3.0};
    cxx::discrete_weights const weights{expected};

    std::vector<double> const values{weights.begin(), weights.end()};
    CHECK(values == expected);
}


TEST_CASE("discrete_weights::operator[] - returns the weight of an event")
{
    cxx::discrete_weights const weights = {1.2, 3.4, 5.6, 7.8};

    CHECK(weights[0] == 1.2);
    CHECK(weights[1] == 3.4);
    CHECK(weights[2] == 5.6);
    CHECK(weights[3] == 7.8);
}


TEST_CASE("discrete_weights::sum - returns the sum of the weights")
{
    cxx::discrete_weights const weights1 = {1.0};
    CHECK(weights1.sum() == Approx(1.0));

    cxx::discrete_weights const weights2 = {1.0, 2.0};
    CHECK(weights2.sum() == Approx(1.0 + 2.0));

    cxx::discrete_weights const weights3 = {1.0, 2.0, 3.0};
    CHECK(weights3.sum() == Approx(1.0 + 2.0 + 3.0));
}


TEST_CASE("discrete_weights::update - updates weight value")
{
    cxx::discrete_weights weights = {1.2, 3.4, 5.6};

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


TEST_CASE("discrete_weights::find - finds the correct event")
{
    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0
    // |----|---------|--------------|
    // |___/|________/|_____________/
    //   0      2            3
    // Note: The element 1 has zero weight, so it won't be found.
    cxx::discrete_weights const weights = {1.0, 0.0, 2.0, 3.0};

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


TEST_CASE("discrete_weights::find - returns edge event for overshoot probe")
{
    // Logically the probe should be in the half-open interval [0, S) where S
    // is the sum of weights. But in practice numerical errors can result in
    // undershoot or overshoot. Here we check robustness against such errors.
    cxx::discrete_weights const weights = {1.0, 2.0, 3.0};

    CHECK(weights.find(-0.1) == 0);
    CHECK(weights.find(-0.0) == 0);
    CHECK(weights.find(6.0) == 2);
    CHECK(weights.find(6.1) == 2);
}


TEST_CASE("discrete_weights::find - finds the correct event after weight update")
{
    // 0.0  1.0  2.0  3.0  4.0  5.0  6.0
    // |----|---------|--------------|
    // |___/|________/|_____________/
    //   0      2            3
    cxx::discrete_weights weights = {1.0, 0.0, 2.0, 3.0};

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


TEST_CASE("discrete_weights - is deserializable")
{
    std::string const source_form = "3 1.2 3.4 5.6";
    std::vector<double> const expected_values = {1.2, 3.4, 5.6};

    cxx::discrete_weights weights;
    std::istringstream ss{source_form};
    ss >> weights;

    // We do not test for exact accuracy.
    CHECK(weights.size() == expected_values.size());

    for (std::size_t i = 0; i < expected_values.size(); i++) {
        CHECK(weights[i] == Approx(expected_values[i]));
    }
}


TEST_CASE("discrete_weights - is serializable")
{
    // Serialization result is indirectly tested via roundtrip because we
    // cannot be sure how parameters are serialized exactly.
    cxx::discrete_weights const origin = {1.2, 3.4, 5.6};
    cxx::discrete_weights roundtrip;

    std::ostringstream os;
    os << origin;
    std::istringstream is{os.str()};
    is >> roundtrip;

    // We do not test for exact accuracy.
    CHECK(roundtrip.size() == origin.size());

    for (std::size_t i = 0; i < origin.size(); i++) {
        CHECK(roundtrip[i] == Approx(origin[i]));
    }
}
