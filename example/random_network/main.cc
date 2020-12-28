// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include <discrete_distribution.hpp>


// A reaction object represents a simple catalytic transformation
//
//   R + C --> P + C  (R: reactant, C: catalyst, P: product)
//
// obeying mass action kinetics.
struct reaction
{
    std::size_t reactant;
    std::size_t catalyst;
    std::size_t product;
    double base_rate;

    inline double
    rate(std::vector<int> const& species) const
    {
        return base_rate * double(species[reactant] * species[catalyst]);
    }
};


int
main()
{
    // We simulate 1M random reactions among 1M species for max 1M steps.
    // The simulation should finish within a couple of seconds.

    // Note: Reducing num_species makes simulation run *slower*. This is
    // because dependency graph gets denser, increasing the cost of weight
    // updates. Sum tree works best in a sparse configuration.

    std::size_t const num_species = 1000000;
    std::size_t const num_reactions = 1000000;
    long const max_steps = 1000000;

    std::default_random_engine random;


    // Initialize species randomly.
    std::vector<int> species;

    while (species.size() < num_species) {
        std::poisson_distribution<int> count;

        species.push_back(1 + count(random));
    }


    // Define random reactions.
    std::vector<reaction> reactions;

    while (reactions.size() < num_reactions) {
        std::uniform_int_distribution<std::size_t> species{0, num_species - 1};
        std::lognormal_distribution<double> base_rate;

        reaction rx;
        rx.reactant = species(random);
        rx.catalyst = species(random);
        rx.product = species(random);
        rx.base_rate = base_rate(random);

        // Avoid self-catalytic reaction as it requires a special-casing for
        // calculating rate.
        if (rx.reactant == rx.catalyst) {
            continue;
        }

        reactions.push_back(rx);
    }


    // Pre-compute which reactions are affected by a change of each species.
    std::vector<std::vector<std::size_t>> dependencies(num_species);

    for (std::size_t rx_index = 0; rx_index < num_reactions; rx_index++) {
        auto const& rx = reactions[rx_index];
        dependencies[rx.reactant].push_back(rx_index);
        dependencies[rx.catalyst].push_back(rx_index);
    }


    // Determine the rates.
    std::vector<double> initial_rates;

    for (auto const& rx : reactions) {
        initial_rates.push_back(rx.rate(species));
    }

    cxx::discrete_distribution<std::size_t> reaction_distr{initial_rates};
    cxx::discrete_weights const& rates = reaction_distr.param();


    // Gillespie algorithm.
    double time = 0;
    long step = 0;

    for (; step < max_steps; step++) {
        if (rates.sum() == 0) {
            break;
        }

        // Determine the wait time.
        std::exponential_distribution<double> wait{rates.sum()};
        time += wait(random);

        // Choose a reaction.
        auto const& rx = reactions[reaction_distr(random)];
        species[rx.reactant] -= 1;
        species[rx.product] += 1;

        // Update affected rates (weights). This is the most expensive part in
        // a dense reaction network.
        for (auto const dep : dependencies[rx.reactant]) {
            reaction_distr.update(dep, reactions[dep].rate(species));
        }
        for (auto const dep : dependencies[rx.product]) {
            reaction_distr.update(dep, reactions[dep].rate(species));
        }
    }

    std::cout << "Stopped after " << step << " reactions\n";
    std::cout << "Simulated time: " << time << '\n';
}
