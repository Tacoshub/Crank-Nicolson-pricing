#pragma once

#include "InterestRate.h"
#include "Tridiag.h"

#include<vector>

class Option {

	int contract_type_;
	int exercise_type_;
	double T_;
	double K_;
	double T0_;
	unsigned int time_mesh_;
	unsigned int spot_mesh_;
	double S0_;
	std::vector<std::pair<double, double>> interest_rate_;
	double volatility_;

	double dT;
	double dS;
	void create_grid();
	std::vector<std::vector<double>> grid;

	InterestRate curve;

	std::vector<double> F;
	double F0, FM;

public:

	Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility);

	double compute_aj(size_t j, unsigned int i);
	double compute_bj(size_t j, unsigned int i);
	double compute_cj(size_t j, unsigned int i);

	Tridiag compute_C(unsigned int i);
	Tridiag compute_D(unsigned int i);
	std::vector<double> compute_K(unsigned int i);

	void solve();
	void display_grid();
	void price();
};