/**
 * @file InterestRate.h
 * @brief Class to represent and manage the interest rate curve.
 */

#pragma once

#include "OptionExceptions.h"
#include <vector>

 /**
  * @class InterestRate
  * @brief Represents an interest rate model with time-dependent rates.
  *
  * This class encapsulates the representation of an interest rate curve,
  * providing functionality to evaluate interest rates at specific times,
  * compute integrals over the curve, and modify rates.
  *
  * The interest rate curve is stored as a vector of (time, rate) pairs,
  * where the time points are assumed to be ordered.
  */
class InterestRate {

    std::vector<std::pair<double, double>> interest_rate_;

public:

    /**
     * @brief Default constructor.
     *
     * Constructs an empty interest rate object with no data.
     */
    InterestRate() {}

    /**
     * @brief Constructs an InterestRate object with a given interest rate curve.
     *
     * The interest rate curve is provided as a vector of time-rate pairs.
     * The time points should be ordered for correct interpolation.
     *
     * @param interest_rate Vector of (time, rate) pairs representing the interest rate curve.
     */
    InterestRate(std::vector<std::pair<double, double>> interest_rate) : interest_rate_(interest_rate) {}

    /**
     * @brief Evaluates the interest rate at a given time.
     *
     * This operator interpolates the interest rate at the specified time \( t \)
     * using linear interpolation. If \( t \) is outside the range of the curve,
     * an exception may be thrown.
     *
     * @param t The time at which to evaluate the interest rate.
     * @return The interpolated interest rate at time \( t \).
     */
    double operator()(double t) const;

    /**
     * @brief Computes the integral of the interest rate curve from 0 to a given time \( t_0 \).
     *
     * Approximates the integral of the interest rate function using numerical methods.
     * Assumes the interest rate curve is defined for all values up to \( t_0 \).
     *
     * @param t0 The upper limit of integration (time).
     * @return The computed integral value from \( 0 \) to \( t_0 \).
     */
    double integral(double t0) const;
};