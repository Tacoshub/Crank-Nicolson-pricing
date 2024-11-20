/**
 * @file InterestRate.cpp
 * @brief Contains the implementation of the InterestRate class and related functions.
 *
 * This file implements the functions for constructinng the interest rate curve using linear interpolation
 * and for computing the integral of the interest rate over a given interval using Simpson's rule.
 */

#include "InterestRate.h"

 /**
  * @brief Calculates the interest rate at time t using linear interpolation between two points.
  *
  * This function interpolates the interest rate for a given time `t` by finding the two closest points
  * in the `interest_rate_` container and performing a linear interpolation between them.
  *
  * @param t The time at which the interest rate is to be calculated.
  * @return The interpolated interest rate at time t.
  * @throws InvalidTime if the time t is outside the bounds of the defined interest rate intervals.
  */
double InterestRate::operator()(double t) const {

	for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
		auto current = *it;
		auto next = *std::next(it, 1);

		if (t >= current.first && t <= next.first) {
			return ((t - current.first) * next.second + (next.first - t) * current.second) / (next.first - current.first);
		}
		else continue;
	}

	throw InvalidTime();
}

/**
 * @brief Calculates the integral of the interest rate function over a specified interval using Simpson's Rule.
 *
 * This function approximates the integral of the interest rate function over the interval [a, b] using
 * Simpson's rule, which is a method of numerical integration. The function first ensures that the number of
 * intervals (n) is even, and then performs the integration.
 *
 * @param a The start of the integration interval.
 * @param b The end of the integration interval.
 * @param n The number of intervals for Simpson's rule (must be even).
 * @return The approximate integral of the interest rate function over [a, b].
 * @throws std::invalid_argument if the number of intervals n is odd.
 */
double InterestRate::integral(double a, double b, int n) const {
	if (n % 2 != 0) {
		throw std::invalid_argument("Number of intervals 'n' must be even for Simpson's rule.");
	}

	double h = (b - a) / n;
	double sum = operator()(a) + operator()(b);

	for (int i = 1; i < n; ++i) {
		double x = a + i * h;
		if (i % 2 == 0) {
			sum += 2 * this->operator()(x);
		}
		else {
			sum += 4 * this->operator()(x);
		}
	}

	sum *= h / 3.0;
	return sum;
}


/*
double InterestRate::integral(double t0, double tf) const
{
	double sum = 0;
	auto stop1 = interest_rate_.begin();
	auto stop2 = stop1;

	for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
		stop1 = it;
		auto current = *it;
		auto next = *std::next(it, 1);

		if (t0 >= current.first && t0 <= next.first) {
			break;
		}
	}

	if (tf <= std::next(stop1, 1)->first) { return ((this->operator()(t0) + this->operator()(tf))) * ((tf - t0)) / 2; }
	else { sum += (this->operator()(t0) + std::next(stop1, 1)->second) * (std::next(stop1, 1)->first - t0)/2; }
	stop2 = std::next(stop1,1);

	for (; stop2 != std::prev(interest_rate_.end(), 1); ++stop2)
	{
		auto current = *stop2;
		auto next = *std::next(stop2, 1);
		if (tf >= current.first && tf <= next.first) {
			sum += (this->operator()(tf) + current.second) * (tf-current.first) / 2;
			return sum;
		}

		sum += (current.second + next.second) * (next.first - current.first) / 2;
		
	}

	return sum;
}
*/
