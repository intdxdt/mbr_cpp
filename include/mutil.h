#include <cmath>

#ifndef MUTIL_MUTIL_H
#define MUTIL_MUTIL_H

const double PRECISION = 12;
const double EPSILON = 1.0e-12;
const double Ln2 = M_LN2;
const double Sqrt2 = M_SQRT2;
const double Pi = M_PI;
const double Tau = 2.0 * Pi;

///Compare two floating point values
[[using gnu : const, always_inline, hot]]
inline bool feq(double a, double b, double eps = EPSILON) {
    return (a == b) || (fabs(a - b) < eps);
}

///Rounds a float to the nearest whole number float
[[using gnu : const, always_inline, hot]]
inline double round_floor(double f) {
    return std::trunc(f + std::copysign(0.5, f));
}

///Rounds a number to the nearest decimal place
[[using gnu : const, always_inline, hot]]
inline double round(double x, int digits = 0) {
    auto m = std::pow(10.0, digits);
    return round_floor(x * m) / m;
}
#endif //MUTIL_MUTIL_H
