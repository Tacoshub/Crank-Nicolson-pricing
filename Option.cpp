#include "Option.h"
#include "Tridiag.h"

#include<iostream>
#include<algorithm>
#include<iomanip>

Option::Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol, double w) : contract_type_(contract_type),  exercise_type_(exercise_type), T_(T), K_(K), T0_(T0), time_mesh_(time_mesh), spot_mesh_(spot_mesh), S0_(S0), interest_rate_(interest_rate), volatility_(volatility), tol_(tol), w_(w) {

	if (contract_type_ != 1 && contract_type_ != -1) throw InvalidContractType(contract_type_);
	if (exercise_type_ != 1 && exercise_type_ != 0) throw InvalidExerciseType(exercise_type_);
	if (T_ < T0 || T < 0) throw InvalidMaturity();
	if (K_ <= 0) throw InvalidStrike(K_);
	if (time_mesh_ <= 0) throw InvalidTimeMesh(time_mesh_);
	if (spot_mesh_ <= 0) throw InvalidSpotMesh(spot_mesh_);
	if (S0_ <= 0) throw InvalidSpot(S0_);
	if (volatility_ <= 0) throw InvalidVolatility(volatility_);

	dT = (T_ - T0_) / (time_mesh_ );
	dS = (5 * S0_) / (spot_mesh_ );

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

std::pair<double, double> Option::compute_K(unsigned int i) {
	double K1 = (compute_aj(1, i - 1) + compute_aj(1, i)) * F0;
	double K2 = (compute_cj(spot_mesh_ - 1, i - 1) + compute_cj(spot_mesh_ - 1, i)) * FM;
	
	return std::make_pair(K1, K2);
}

double Option::compute_K_american(unsigned int i) {
	return compute_aj(1, i - 1) * (std::exp(-curve.integral((i-1)*dT, T_)) + std::exp(-curve.integral(i*dT, T_)))*F0;
}

std::vector<double> operator+(std::vector<double> v1, std::pair<double, double> p2) {
	v1[0] += p2.first;
	v1[v1.size() - 1] += p2.second;
	return v1;
}

std::vector<double> operator-(std::vector<double> v1, std::vector<double> v2) {
	size_t n = v1.size();
	std::vector<double> res(n);
	for (size_t ii = 0; ii < n; ii++) {
		res[ii] = v1[ii] - v2[ii];
	}
	return res;
}

double norm(std::vector<double> v1) {
	size_t n = v1.size();
	double som = 0;
	for (size_t ii = 0; ii < n; ii++) {
		som += v1[ii] * v1[ii];
	}
	return std::sqrt(som);
}

std::vector<double> operator+(std::vector<double> v1, double p2) {
	v1[0] += p2;
	return v1;
}

void Option::european_price() {
	Tridiag C, D;
	std::pair<double, double> K;
	std::vector<double> RHS;
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
		grid[zz][jj - 1] = std::exp(-curve.integral(dT * (jj - 1), T_)) * FM;
	}
}

void Option::american_price(int ct) {
	if (ct == 1) european_price();
	else {
		Tridiag D;
		double K;
		std::vector<double> RHS;
		double Sk;
		size_t ii, zz;
		for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
			D = compute_D(jj);
			K = compute_K_american(jj);
			RHS = D * F + K;
			double error = 1000000;
			std::vector<double> F_tmp(spot_mesh_ - 1);
			while (error > tol_) {
				Sk = dS;
				F_tmp[0] = std::max(std::max(K_ - Sk, 0.0), F[0] + (w_ / (1 - compute_bj(1, jj))) * (RHS[0] - (1 - compute_bj(1, jj)) * F[0] + compute_cj(1, jj) * F[1]));
				ii = 1;
				for (; ii < spot_mesh_ - 2; ii++) {
					Sk += dS;
					F_tmp[ii] = std::max(std::max(K_ - Sk, 0.0), F[ii] + (w_ / (1 - compute_bj(ii + 1, jj))) * (RHS[ii] + compute_aj(ii + 1, jj) * F[ii - 1] - (1 - compute_bj(ii + 1, jj)) * F[ii] + compute_cj(ii + 1, jj) * F[ii + 1]));
				}
				Sk += dS;
				F_tmp[ii] = std::max(std::max(K_ - Sk, 0.0), F[ii] + (w_ / (1 - compute_bj(ii + 1, jj))) * (RHS[ii] + compute_aj(ii + 1, jj) * F[ii - 1] - (1 - compute_bj(ii + 1, jj)) * F[ii]));
				error = norm(F - F_tmp);
				F = F_tmp;
			}
			grid[0][jj - 1] = F0;
			for (zz = 1; zz < spot_mesh_; zz++) {
				grid[zz][jj - 1] = F[zz - 1];
			}
			grid[zz][jj - 1] = std::exp(-curve.integral(dT * (jj - 1), T_)) * FM;
			std::cout << jj << std::endl;
		}
	}
}

void Option::solve() {
	double Sk = 0;
	size_t ii = 0;
	F.resize(spot_mesh_ - 1);
	for (; ii < spot_mesh_; ii++) {
		grid[ii][time_mesh_ - 1] = std::max(contract_type_ * (Sk - K_), 0.0);
		if (ii != 0) F[ii-1] = (grid[ii][time_mesh_ - 1]);
		Sk += dS;
	}
	grid[ii][time_mesh_ - 1] = FM;

	if (exercise_type_) {
		european_price();
	}
	else {
		american_price(contract_type_);
	}
}

void Option::price() {
	std::cout << grid[S0_ / dS][0];
}

void Option::display_grid() {
	std::cout << std::fixed << std::setprecision(3);
	for (size_t ii = 0; ii <= spot_mesh_; ii++) {
		for (size_t jj = 0; jj < time_mesh_; jj++) {
			std::cout << std::setw(8) << grid[ii][jj] << " ";
		}
		std::cout << '\n';
	}
}