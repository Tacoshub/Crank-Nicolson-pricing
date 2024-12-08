/**
 * @file InterestRate.cpp
 * @brief Contains various methods to handle the interest rate curve, including evaluation, integration, and modification.
 */

#include "InterestRate.h"
#include <iostream>

 /**
  * @brief Evaluates the interest rate at a specified time using linear interpolation.
  *
  * This function locates the interval \([t_i, t_{i+1}]\) where the input time \( t \) lies within
  * the defined time points of the interest rate curve. It then performs linear interpolation
  * between the corresponding interest rates \( r_i \) and \( r_{i+1} \) to compute the interpolated value.
  *
  * If \( t \) is outside the range of the defined intervals, the function returns the interest rate
  * corresponding to the latest time point.
  *
  * @param t The time at which the interest rate is to be evaluated.
  * @return The interpolated interest rate at the specified time \( t \).
  */
double InterestRate::operator()(double t) const {
    for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
        auto current = *it;
        auto next = *std::next(it, 1);

        if (t >= current.first && t <= next.first) {
            return ((t - current.first) * next.second + (next.first - t) * current.second) / (next.first - current.first);
        }
    }
    return interest_rate_.back().second;
}

/**
 * @brief Computes the area under a line segment of the interest rate curve.
 *
 * Handles both positive and negative rates and computes the area under the curve
 * for a segment defined by two points \((t_1, r_1)\) and \((t_2, r_2)\).
 * If the rates have opposite signs, splits the area at the zero crossing.
 *
 * @param r1 Interest rate at the starting point \( t_1 \).
 * @param r2 Interest rate at the ending point \( t_2 \).
 * @param t1 The starting time of the segment.
 * @param t2 The ending time of the segment.
 * @return The computed area under the curve between \( t_1 \) and \( t_2 \).
 */
double support_integral(double r1, double r2, double t1, double t2) {
    if ((r1 >= 0 && r2 >= 0) || (r1 <= 0 && r2 <= 0)) {
        return ((r1 + r2) * (t2 - t1)) / 2;
    }
    else {
        double x = (t1 * r2 - t2 * r1) / (r2 - r1);
        double tri1 = ((x - t1) * r1 / 2);
        double tri2 = ((t2 - x) * r2 / 2);
        return tri1 + tri2;
    }
}

/**
 * @brief Computes the integral of the interest rate curve from 0 to a given time \( t_0 \).
 *
 * Calculates the integral of the interest rate curve by summing up the areas
 * under each line segment. Uses `support_integral` to handle each segment.
 * If \( t_0 \) lies within a segment, computes the partial area for that segment.
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