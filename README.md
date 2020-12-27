# Efficient discrete distribution for C++ &lt;random&gt;

[![Test Status][test-badge]][test-url]
[![Boost License][license-badge]](LICENSE.txt)
![C++11,14,17,20][cxx-badge]

Header-only library providing `discrete_distribution` class for C++11 and later.
The class allows efficient modification of event weights, making it suitable
for dynamic simulations like [Kinetic Monte Carlo][kmc] and [Gillespie
algorithm][gillespie].

[kmc]: https://en.wikipedia.org/wiki/Kinetic_Monte_Carlo
[gillespie]: https://en.wikipedia.org/wiki/Gillespie_algorithm

[test-badge]: https://github.com/snsinfu/cxx-distr/workflows/test/badge.svg
[test-url]: https://github.com/snsinfu/cxx-distr/actions?query=workflow%3Atest
[cxx-badge]: https://img.shields.io/badge/C%2B%2B-11%2F14%2F17%2F20-orange.svg
[license-badge]: https://img.shields.io/badge/license-Boost-blue.svg

- [License](#license)


## License

Boost Software License, Version 1.0.
