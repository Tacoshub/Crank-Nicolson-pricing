#include "InterestRate.h"

double InterestRate::operator()(double t) const {

	for (auto it = interest_rate_.begin(); it != std::prev(interest_rate_.end(), 1); ++it) {
		auto current = *it;
		auto next = *std::next(it, 1);

		if (t >= current.first && t <= next.first) {
			return ((t - current.first) * next.second + (next.first - t) * current.second) / (next.first - current.first);
		}
		else continue;
	}
	throw "cazzo";

}