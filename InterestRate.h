/**
 * @file OptionExceptions.h
 * @brief contains various methods to deal with the interest rate curve
 */
#pragma once

#include "OptionExceptions.h"
#include <vector>

/**
 * @class InterestRate
 * @brief Represents an interest rate model with time-dependent rates.
 *
 * This class provides functionality to evaluate interest rates, compute integrals, and modify rates.
 */
class InterestRate {

    std::vector<std::pair<double, double>> interest_rate_; ///< Vector of (time, rate) pairs representing the interest rate curve.

public:

    /**
     * @brief Default constructor.
     *
     * Creates an empty interest rate object.
     */
    InterestRate() {}

    /**
     * @brief Constructs an InterestRate object with a given interest rate curve.
     *
     * @param interest_rate Vector of (time, rate) pairs.
     */
    InterestRate(std::vector<std::pair<double, double>> interest_rate) : interest_rate_(interest_rate) {}

    /**
     * @brief Evaluates the interest rate at a given time.
     *
     * This operator interpolates the interest rate based on the provided time, \( t \).
     *
     * @param t The time at which to evaluate the interest rate.
     * @return The interest rate at time \( t \).
     */
    double operator()(double t) const;

    /**
     * @brief Computes the integral of the interest rate curve from 0 to a given time \( t_0 \).
     *
     * Uses numerical integration to approximate the integral of the interest rate function over the specified interval.
     *
     * @param t0 The upper limit of integration.
     * @return The integral value from 0 to \( t_0 \).
     */
    double integral(double t0) const;

    /**
     * @brief Increments all interest rates by a given value.
     *
     * Adds \( h \) to the rates of all time points in the interest rate curve.
     *
     * @param h The value to increment the rates by.
     */
    void operator+=(double h);

    /**
     * @brief Decrements all interest rates by a given value.
     *
     * Subtracts \( h \) from the rates of all time points in the interest rate curve.
     *
     * @param h The value to decrement the rates by.
     */
    void operator-=(double h);
};
