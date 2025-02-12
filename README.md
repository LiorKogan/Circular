# Circular

A C++11 library for circular values (angles, time-of-day, etc.) mathematics and statistics

Copyright © 2015 Lior Kogan (koganlior1 [at] gmail [dot] com)

Released under the Apache License, Version 2.0

--

Many scientific and engineering problems involve circular real numbers. These numbers usually represent angular measurements (e.g., azimuth) or cyclical timestamps (e.g., time of day), but may represent other circular quantities. Circular reals are very common in physics, geodesics, and navigation, but also appear in fields such as psychology and criminology (time-of-day statistics), bird-watching, and biology (directional statistics and time-of-year statistics).

The mathematics and statistics of circular reals are tricky and error-prone, both for simple operations such as addition and subtraction and for more complex operations such as average, median, parameter estimation, and interpolations. In addition, different ranges and zero-values are used. For angles, [-π,π), [0,2π), [-180,180), and [0,360) are often used, and for time-of-day, [0,24), [0,24∙60∙60) and other ranges.

Scope:

- Theoretical foundations (see documentation)
- A class for storing and computing with circular reals
- Operators: comparison, circular arithmetic, trigonometric
- Conversion between different types of circular reals
- Conversion between circular reals and reals
- Statistical distributions: wrapped normal, truncated normal, wrapped truncated normal 
- Circular average, weighted average, and median
- Circular parameter estimation based on noisy measurements 
- Interpolation and average estimation of a sampled continuous-time circular signal

[Description and documentation](https://github.com/LiorKogan/Circular/blob/main/Doc/Circular.pdf)

[CodeProject's Best C++ article, May 2011](https://www.codeproject.com/Articles/190833/Circular-Values-Math-and-Statistics-with-Cplusplus)
