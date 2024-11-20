/**
 * @file InterestRate.h
 * @brief Defines the InterestRate class, which provides functionality for interest rate interpolation and integration.
 *
 * This file contains the declaration of the `InterestRate` class, which models an interest rate as a
 * function of time. It provides methods for interpolating interest rates at a given time and for
 * calculating the integral of the interest rate function over a specified time interval.
 */

#pragma once

#include "OptionExceptions.h"
#include <vector>

 /**
  * @class InterestRate
  * @brief Represents a collection of interest rate data and provides methods for interpolation and integration.
  *
  * The `InterestRate` class stores a set of interest rate data as pairs of time and rate. It offers
  * functionality to interpolate interest rates at any given time using linear interpolation, as well
  * as to compute the integral of the interest rate function over a specified interval using Simpson's rule.
  */
class InterestRate {

    /// A vector of pairs, where each pair represents a time and its corresponding interest rate.
    std::vector<std::pair<double, double>> interest_rate_;

public:

    /**
     * @brief Default constructor.
     *
     * This constructor initializes an empty `InterestRate` object with no data.
     */
    InterestRate() {}

    /**
     * @brief Parameterized constructor.
     *
     * This constructor initializes the `InterestRate` object with a set of interest rate data.
     *
     * @param interest_rate A vector of pairs where each pair contains a time and its corresponding interest rate.
     */
    InterestRate(std::vector<std::pair<double, double>> interest_rate) : interest_rate_(interest_rate) {}

    /**
     * @brief Interpolates the interest rate at a given time.
     *
     * This function calculates the interest rate at a specific time `t` by performing linear interpolation
     * between two points in the `interest_rate_` container.
     *
     * @param t The time at which the interest rate is to be calculated.
     * @return The interpolated interest rate at time t.
     * @throws InvalidTime if the time t is outside the bounds of the defined interest rate intervals.
     */
    double operator()(double t) const;

    /**
     * @brief Calculates the integral of the interest rate function over a given interval using Simpson's Rule.
     *
     * This function approximates the integral of the interest rate function over the interval [t0, tf] using
     * Simpson's rule. The function ensures that the number of intervals (n) is even, and it calculates the
     * integral accordingly.
     *
     * @param t0 The start of the integration interval.
     * @param tf The end of the integration interval.
     * @param n The number of intervals for Simpson's rule (defaults to 1000, must be even).
     * @return The approximate integral of the interest rate function over [t0, tf].
     * @throws std::invalid_argument if the number of intervals n is odd.
     */
    double integral(double t0, double tf, int n = 1000) const;
};
