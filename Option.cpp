#include "Option.h"
#include "Tridiag.h"

#include<iostream>
#include<algorithm>
#include<iomanip>

Option::Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility) {
	contract_type_ = contract_type;
	exercise_type = exercise_type;
	T_ = T;
	K_ = K;
	T0_ = T0;
	time_mesh_ = time_mesh;
	spot_mesh_ = spot_mesh;
	S0_ = S0;
	interest_rate_ = interest_rate;
	volatility_ = volatility;

	dT = (T_ - T0_) / (time_mesh_ - 1);
	dS = (4 * S0_) / (spot_mesh_ - 1);
	std::vector<std::vector<double>> grid;
	create_grid();

	curve = InterestRate(interest_rate_);

	if (contract_type == 1) { F0 = 0, FM = 5*S0 - K; }
	else { F0 = K, FM = 0; }	
	solve();
}

void Option::create_grid() {
	std::vector<std::vector<double>> matrix(spot_mesh_ + 1, std::vector<double>(time_mesh_, 0.0));
	grid = matrix;
}

double Option::compute_aj(size_t j, unsigned int i) {
	double aj = (dT / 4) * (volatility_*volatility_ * j * j - curve(dT * i) * j);
	return aj;
}

double Option::compute_bj(size_t j, unsigned int i) {
	double bj = -(dT / 2) * (volatility_ * volatility_ * j * j + curve(dT * i));
	return bj;
}

double Option::compute_cj(size_t j, unsigned int i) {
	double cj = (dT / 4) * (volatility_ * volatility_ * j * j + curve(dT * i) * j);
	return cj;
}

Tridiag Option::compute_C(unsigned int i) {
	std::vector<double> a, b, c;
	size_t jj = 1;
	for (; jj < spot_mesh_ - 1; jj++) {
		b.push_back(1 - compute_bj(jj, i));
		c.push_back(-compute_cj(jj, i));
		a.push_back(-compute_aj(jj + 1, i));
	}
	b.push_back(1 - compute_bj(jj, i));
	return Tridiag(a, b, c);	
}

Tridiag Option::compute_D(unsigned int i) {
	std::vector<double> a, b, c;
	size_t jj = 1;
	for (; jj < spot_mesh_ - 1; jj++) {
		b.push_back(1 + compute_bj(jj, i));
		c.push_back(compute_cj(jj, i));
		a.push_back(compute_aj(jj + 1, i));
	}
	b.push_back(1 + compute_bj(jj, i));
	return Tridiag(a, b, c);
}

std::vector<double> Option::compute_K(unsigned int i) {
	std::vector<double> K(spot_mesh_ - 1, 0.0);
	K[0] = (compute_aj(1, i - 1) + compute_aj(1, i)) * F0;
	K[spot_mesh_ - 2] = (compute_cj(spot_mesh_ - 1, i - 1) + compute_cj(spot_mesh_ - 1, i)) * FM;
	return K;
}

std::vector<double> operator+(std::vector<double> v1, std::vector<double> v2) {
	std::vector<double> res(v1.size());
	for (size_t ii = 0; ii < v1.size(); ii++) {
		res[ii] = v1[ii] + v2[ii];
	}
	return res;
}

void Option::solve() {
	double Sk = S0_;
	size_t ii = 0;
	for (; ii < spot_mesh_; ii++) {
		grid[ii][time_mesh_ - 1] = std::max(contract_type_ * (Sk - K_), 0.0);
		if (ii != 0) F.push_back(grid[ii][time_mesh_ - 1]);
		Sk += dS;
	}
	grid[ii][time_mesh_ - 1] = FM;
	Tridiag C, D;
	std::vector<double> K, RHS;
	size_t zz;
	for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
		C = compute_C(jj);
		D = compute_D(jj);
		K = compute_K(jj);
		RHS = D * F + K;
		F = C.solve(RHS);
		grid[0][jj - 1] = F0;
		for (zz = 1; zz < spot_mesh_; zz++) {
			grid[zz][jj - 1] = F[zz - 1];
		}
		grid[zz][jj - 1] = FM;
	}
}

void Option::price() {
	std::cout << grid[S0_ / dS][0];
}

void Option::display_grid() {
	std::cout << std::fixed << std::setprecision(4);
	for (size_t ii = 0; ii <= spot_mesh_; ii++) {
		for (size_t jj = 0; jj < time_mesh_; jj++) {
			std::cout << std::setw(10) << grid[ii][jj] << " ";
		}
		std::cout << '\n';
	}
}