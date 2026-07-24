# Circular

A C++20 library for mathematics and statistics on circular values, such as angles and time of day.

Copyright © 2015 Lior Kogan (koganlior1 [at] gmail [dot] com)

Released under the Apache License, Version 2.0

[Primary maintained copy](https://github.com/LiorKogan/Circular) <br>
[Also at](https://github.com/DRNadler/Circular)

### Overview

Many scientific and engineering problems involve circular real numbers - values defined on a cycle rather than a linear scale. Common examples include angular measurements (e.g., azimuth or heading) and cyclical timestamps (e.g., time of day). Circular values are prevalent in physics, geodesy, and navigation, but also appear in fields such as psychology and criminology (time-of-day statistics), biology and birdwatching (directional statistics), and seasonal analysis.

Working with circular values is subtle and error-prone due to the wrap-around property: when values exceed the range boundaries, they “loop back,” making ordinary arithmetic, comparisons, and statistics non-trivial. Even basic operations like addition and subtraction require care, while advanced tasks—such as averaging, median computation, parameter estimation, and interpolation—are especially tricky. The variety of commonly used ranges and zero conventions (e.g., [-π, π), [0, 2π), [-180°, 180°), [0°, 360°), [0, 24), [0, 24·60·60), etc.) further complicates calculations.

This library provides a principled, well-documented foundation for working with circular real numbers robustly and expressively.

### Scope:

- Theoretical foundations (see documentation)
- A class for representing and computing with circular real values
- Comparison, circular arithmetic, and trigonometric operators
- Conversion between different circular domains and conventions
- Conversion between circular and linear real values
- Circular probability distributions: Wrapped normal, Truncated normal, Wrapped truncated normal
- Average, weighted average, and median
- Parameter estimation from noisy measurements
- Interpolation and average estimation of sampled continuous-time circular signals

[Description and documentation](https://github.com/LiorKogan/Circular/blob/main/Doc/Circular.pdf)

[CodeProject's Best C++ article, May 2011](https://web.archive.org/web/20191016064937/https://www.codeproject.com/Articles/190833/Circular-Values-Math-and-Statistics-with-Cplusplus)
