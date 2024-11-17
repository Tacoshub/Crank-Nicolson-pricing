#pragma once

#include<vector>

class InterestRate {

	std::vector<std::pair<double, double>> interest_rate_;

public:

	InterestRate() {}
	InterestRate(std::vector<std::pair<double, double>> interest_rate) : interest_rate_(interest_rate) {}

	double operator()(double t) const;
	double integral(double t0, double tf)const;
	
};
