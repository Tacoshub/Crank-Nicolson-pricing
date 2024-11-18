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
	throw InvalidTime();

}

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
