#include "InterestRate.h"

#include<iostream>

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

double InterestRate::integral(double t0) const {
	double res = 0;
	for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
		std::pair<double, double> curr = *it;
		std::pair<double, double> next = *std::next(it, 1);
		if (t0 >= curr.first && t0 <= next.first) {
			res += support_integral(operator()(t0), next.second, t0, next.first);
		}
		else if (curr.first >= t0) res += support_integral(curr.second, next.second, curr.first, next.first);
	}
	return res;
}