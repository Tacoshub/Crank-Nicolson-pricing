#pragma once

#include "InterestRate.h"
#include "OptionExceptions.h"
#include "Tridiag.h"

#include<vector>

class Option {

	int contract_type_;
	bool exercise_type_;
	double T_, K_, T0_, S0_, volatility_;
	unsigned int time_mesh_, spot_mesh_;

	std::vector<std::pair<double, double>> interest_rate_;
	InterestRate curve;

	double dT, dS, F0, FM;
	std::vector<std::vector<double>> grid;
	std::vector<double> F;

	void create_grid();

	void european_price();
	void american_price();

	double tol_;
	double w_;

public:

	Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol = 0.01, double w = 1.2);

	std::vector<double> compute_aj(unsigned int i);
	std::vector<double> compute_bj(unsigned int i);
	std::vector<double> compute_cj(unsigned int i);

	Tridiag compute_C(std::vector<double> a, std::vector<double> b, std::vector<double> c);
	Tridiag compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c);
	std::pair<double, double> compute_K(unsigned int i);
	double compute_K_american(unsigned int i);

	void solve();
	void display_grid();
	void price();
};