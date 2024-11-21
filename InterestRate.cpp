/**
 * @file InterestRate.cpp
 * @brief contains various methods to deal with the interest rate curve
 */
#include "InterestRate.h"
#include <iostream>

/**
 * @brief Evaluates the interest rate at a given time using linear interpolation.
 *
 * Finds the interval \([t_i, t_{i+1}]\) where \( t \) lies and performs linear interpolation between the corresponding rates.
 *
 * @param t The time at which to evaluate the interest rate.
 * @return The interpolated interest rate at time \( t \).
 * @throw InvalidTime If \( t \) is outside the bounds of the interest rate curve.
 */
double InterestRate::operator()(double t) const {
    for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
        auto current = *it;
        auto next = *std::next(it, 1);

        if (t >= current.first && t <= next.first) {
            return ((t - current.first) * next.second + (next.first - t) * current.second) / (next.first - current.first);
        }
    }
    throw InvalidTime();
}

/**
 * @brief Computes the area under a line segment in the interest rate curve between two time points.
 *
 * Handles cases where the rates \( r_1 \) and \( r_2 \) have opposite signs by splitting the integral at the zero crossing.
 *
 * @param r1 Interest rate at \( t_1 \).
 * @param r2 Interest rate at \( t_2 \).
 * @param t1 Starting time of the segment.
 * @param t2 Ending time of the segment.
 * @return The computed area under the curve between \( t_1 \) and \( t_2 \).
 */
double support_integral(double r1, double r2, double t1, double t2) {
    if ((r1 >= 0 && r2 >= 0) || (r1 <= 0 && r2 <= 0)) {
        return (abs(r1 + r2) * (t2 - t1)) / 2;
    }
    else {
        double x = (t1 * r2 - t2 * r1) / (r2 - r1);
        double tri1 = abs((x - t1) * r1 / 2);
        double tri2 = abs((t2 - x) * r2 / 2);
        return tri1 + tri2;
    }
}

/**
 * @brief Computes the integral of the interest rate curve from 0 to a given time \( t_0 \).
 *
 * Uses the `support_integral` function to compute the area under each line segment of the curve.
 * Handles cases where \( t_0 \) lies within an interval by partially integrating that segment.
 *
 * @param t0 The upper limit of integration.
 * @return The integral value from 0 to \( t_0 \).
 */
double InterestRate::integral(double t0) const {
    double res = 0;
    for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
        std::pair<double, double> curr = *it;
        std::pair<double, double> next = *std::next(it, 1);
        if (t0 >= curr.first && t0 <= next.first) {
            res += support_integral(operator()(t0), next.second, t0, next.first);
        }
        else if (curr.first >= t0) {
            res += support_integral(curr.second, next.second, curr.first, next.first);
        }
    }
    return res;
}

/**
 * @brief Increments all interest rates in the curve by a constant value.
 *
 * @param h The value to add to all interest rates.
 */
void InterestRate::operator+=(double h) {
    size_t n = interest_rate_.size();
    for (size_t ii = 0; ii < n; ii++) {
        interest_rate_[ii].second += h;
    }
}

/**
 * @brief Decrements all interest rates in the curve by a constant value.
 *
 * @param h The value to subtract from all interest rates.
 */
void InterestRate::operator-=(double h) {
    size_t n = interest_rate_.size();
    for (size_t ii = 0; ii < n; ii++) {
        interest_rate_[ii].second -= h;
    }
}
