# Efficient discrete random distribution for C++

[![Test Status][test-badge]][test-url]
![C++11][cxx-badge]
[![Boost License][license-badge]](LICENSE.txt)

Header-only library providing `cxx::discrete_distribution` class for C++11
and later. The class uses array-based sum tree to allow efficient sampling
and updates of discrete events, making it suitable for dynamic simulations like
[Kinetic Monte Carlo][kmc] and [Gillespie algorithm][gillespie].

[kmc]: https://en.wikipedia.org/wiki/Kinetic_Monte_Carlo
[gillespie]: https://en.wikipedia.org/wiki/Gillespie_algorithm

[test-badge]: https://github.com/snsinfu/cxx-distr/workflows/test/badge.svg
[test-url]: https://github.com/snsinfu/cxx-distr/actions?query=workflow%3Atest
[cxx-badge]: https://img.shields.io/badge/C%2B%2B-11-orange.svg
[license-badge]: https://img.shields.io/badge/license-Boost-blue.svg

- [Usage](#usage)
- [Testing](#testing)
- [Project Status](#project-status)
- [License](#license)


## Usage

Download [discrete_distribution.hpp][hpp] and include in your program. The
library is single header-only and has zero dependency.

```c++
#include <discrete_distribution.hpp>

int main()
{
    // Distribution of four events with these weights.
    cxx::discrete_distribution distr = {1.2, 3.4, 5.6, 7.8};
    std::mt19937_64 random;

    // 0, 1, 2 or 3 is chosen.
    std::size_t choice = distr(random);

    // Change the weight of the event 1 to zero.
    distr.update(1, 0.0);

    // Now event 1 will not be chosen because it has zero weight.
    choice = distr(random);
}
```

[hpp]: https://github.com/snsinfu/cxx-distr/raw/master/include/discrete_distribution.hpp


## Testing

To run unit tests:

```sh
git clone https://github.com/snsinfu/cxx-distr.git
cd cxx-distr/test
make
```

Also you can test example program:

```sh
cd cxx-distr/example
make -C gillespie
```


## Project Status

Designing API.


## License

Boost Software License, Version 1.0.
