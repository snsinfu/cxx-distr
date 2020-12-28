// Copyright snsinfu 2020.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include <discrete_distribution.hpp>


int main()
{
    // Simulate circular reaction network of N species:
    //
    //       k      k      k      k        k        k      k
    //  ... ---> 0 ---> 1 ---> 2 ---> ... ---> N-1 ---> 0 ---> ...
    //
    // There are N reactions and we name i ---> i+1 the i-th reaction.

    // We simulate reactions of 100k species for 100k steps. This looks massive,
    // but the simulation finishes instantly thanks to the efficiency of
    // `cxx::discrete_distribution`. Try changing `use_std_distribution` to
    // true and see the difference.

    std::size_t const num_species = 100000;
    double const base_rate = 0.1;
    int const initial_count = 5;
    long const simulation_steps = 100000;
    bool const use_std_distribution = false;

    std::mt19937_64 random;

    // Discrete distribution of N reactions. We will update the rates
    // (weights) based on the number of species.
    std::vector<double> const initial_rate(num_species, 0.0);
    cxx::discrete_distribution<std::size_t> reaction_distr{initial_rate};

    // Simulation state.
    std::vector<int> species(num_species);
    double time = 0;

    // Initial condition.
    species[0] = initial_count;
    reaction_distr.update(0, base_rate * double(species[0]));

    for (long step = 1; step <= simulation_steps; step++) {
        // Gillespie algorithm.

        // Time delay. Conveniently, the sum of reaction rates = probability
        // weights is freely available as `reaction_distr.sum()`.
        std::exponential_distribution<double> delay_distr{reaction_distr.sum()};
        time += delay_distr(random);

        // Choose reaction.
        std::size_t reaction;

        if (use_std_distribution) {
            // Standard algorithm is orders of magnitude slower!
            auto const& weights = reaction_distr.param();
            std::discrete_distribution<std::size_t> discrete{
                weights.begin(), weights.end()
            };
            reaction = discrete(random);
        } else {
            reaction = reaction_distr(random);
        }

        // i-th reaction: i ---> i+1 .
        auto const reactant = reaction;
        auto const product = (reaction + 1) % num_species;
        species[reactant] -= 1;
        species[product] += 1;

        // Reaction rate changes.
        reaction_distr.update(reactant, base_rate * double(species[reactant]));
        reaction_distr.update(product, base_rate * double(species[product]));
    }

    // Print the result.
    std::cout << time << '\t' << species[0] << '\n';
}
