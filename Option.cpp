#include "Option.h"
#include "Tridiag.h"

#include<iostream>
#include<algorithm>
#include<iomanip>


std::vector<double> operator*(double k, std::vector<double> v) {
	size_t n = v.size();
	for (size_t ii = 0; ii < n; ii++) {
		v[ii] *= k;
	}
	return v;
}

std::vector<double> operator-(double k, std::vector<double> v) {
	size_t n = v.size();
	for (size_t ii = 0; ii < n; ii++) {
		v[ii] = k - v[ii];
	}
	return v;
}

std::vector<double> operator+(double k, std::vector<double> v) {
	size_t n = v.size();
	for (size_t ii = 0; ii < n; ii++) {
		v[ii] = k + v[ii];
	}
	return v;
}

std::vector<double> operator-(std::vector<double> v1, std::vector<double> v2) {
	size_t n = v1.size();
	std::vector<double> res(n);
	for (size_t ii = 0; ii < n; ii++) {
		res[ii] = v1[ii] - v2[ii];
	}
	return res;
}

std::vector<double> operator+(std::vector<double> v1, double p2) {
	v1[0] += p2;
	return v1;
}

std::vector<double> operator+(std::vector<double> v1, std::pair<double, double> p2) {
	v1[0] += p2.first;
	v1[v1.size() - 1] += p2.second;
	return v1;
}

double norm(std::vector<double> v1) {
	size_t n = v1.size();
	double som = 0;
	for (size_t ii = 0; ii < n; ii++) {
		som += v1[ii] * v1[ii];
	}
	return std::sqrt(som);
}





Option::Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol, double w) : contract_type_(contract_type), exercise_type_(exercise_type), T_(T), K_(K), T0_(T0), time_mesh_(time_mesh), spot_mesh_(spot_mesh), S0_(S0), interest_rate_(interest_rate), volatility_(volatility), tol_(tol), w_(w) {

	if (contract_type_ != 1 && contract_type_ != -1) throw InvalidContractType(contract_type_);
	if (exercise_type_ != 1 && exercise_type_ != 0) throw InvalidExerciseType(exercise_type_);
	if (T_ < T0 || T < 0) throw InvalidMaturity();
	if (K_ <= 0) throw InvalidStrike(K_);
	if (time_mesh_ <= 0) throw InvalidTimeMesh(time_mesh_);
	if (spot_mesh_ <= 0) throw InvalidSpotMesh(spot_mesh_);
	if (S0_ <= 0) throw InvalidSpot(S0_);
	if (volatility_ <= 0) throw InvalidVolatility(volatility_);

	dT = (T_ - T0_) / (time_mesh_);
	dS = (5 * S0_) / (spot_mesh_);

	create_grid();

	curve = InterestRate(interest_rate_);

	if (contract_type == 1) { F0 = 0, FM = 5 * S0; }
	else { F0 = K, FM = 0; }

	solve();

}

void Option::create_grid() {
	std::vector<std::vector<double>> matrix(spot_mesh_ + 1, std::vector<double>(time_mesh_, 0.0));
	grid = matrix;
}

std::vector<double> Option::compute_aj(unsigned int i) {
	std::vector<double> aj(spot_mesh_ - 2);
	for (size_t jj = 2; jj < spot_mesh_; jj++) {
		aj[jj - 2] = (dT / 4) * (volatility_ * volatility_ * jj * jj - curve(dT * i) * jj);
	}
	return aj;
}

std::vector<double> Option::compute_bj(unsigned int i) {
	std::vector<double> bj(spot_mesh_ - 1);
	for (size_t jj = 1; jj < spot_mesh_; jj++) {
		bj[jj - 1] = -(dT / 2) * (volatility_ * volatility_ * jj * jj + curve(dT * i));
	}
	return bj;
}

std::vector<double> Option::compute_cj(unsigned int i) {
	std::vector<double> cj(spot_mesh_ - 2);
	for (size_t jj = 1; jj < spot_mesh_ - 1; jj++) {
		cj[jj - 1] = (dT / 4) * (volatility_ * volatility_ * jj * jj + curve(dT * i) * jj);
	}
	return cj;
}

Tridiag Option::compute_C(std::vector<double> a, std::vector<double> b, std::vector<double> c) {
	return Tridiag(-1.0 * a, 1.0 - b, -1.0 * c);
}

Tridiag Option::compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c) {
	return Tridiag(a, 1.0 + b, c);
}

std::pair<double, double> Option::compute_K(unsigned int i) {
	double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);
	double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);
	double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) + a1_curr * F0 * std::exp(-curve.integral(dT * i));

	double cm_prec = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * (i - 1)) * (spot_mesh_ - 1));
	double cm_curr = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * i) * (spot_mesh_ - 1));
	double K2 = cm_prec * (FM - K_ * std::exp(-curve.integral(dT * (i - 1)))) * (contract_type_ == 1) + cm_curr * (FM - K_ * std::exp(-curve.integral(dT * (i - 1)))) * (contract_type_ == 1);

	return std::make_pair(K1, K2);
}

double Option::compute_K_american(unsigned int i) {
	double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);
	double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);
	double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) + a1_curr * F0 * std::exp(-curve.integral(dT * i));

	return K1;
}





void Option::european_price() {
	Tridiag C, D;
	std::pair<double, double> K;
	std::vector<double> a, b, c, RHS;
	size_t zz;
	for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
		a = compute_aj(jj), b = compute_bj(jj), c = compute_cj(jj);
		C = compute_C(a, b, c), D = compute_D(a, b, c);
		K = compute_K(jj);
		RHS = D * F + K;
		F = C.solve(RHS);
		grid[0][jj - 1] = F0 * std::exp(-curve.integral(dT * (jj - 1)));
		for (zz = 1; zz < spot_mesh_; zz++) {
			grid[zz][jj - 1] = F[zz - 1];
		}
		grid[zz][jj - 1] = (FM - K_ * std::exp(-curve.integral(dT * (jj - 1)))) * (contract_type_ == 1);
	}
}

void Option::american_price() {
	std::vector<double> a, b, c;
	Tridiag D;
	double K, Sk;
	std::vector<double> RHS;
	size_t ii, zz;
	for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
		a = compute_aj(jj), b = compute_bj(jj), c = compute_cj(jj);
		D = compute_D(a, b, c);
		K = compute_K_american(jj);
		RHS = D * F + K;
		double error = 1000000;
		std::vector<double> F_tmp(spot_mesh_ - 1);
		while (error > tol_) {
			Sk = dS;
			F_tmp[0] = std::max(std::max(contract_type_ * (Sk - K_), 0.0), F[0] + (w_ / (1 - b[0])) * (RHS[0] - (1 - b[0]) * F[0] + c[0] * F[1]));
			ii = 1;
			for (; ii < spot_mesh_ - 2; ii++) {
				Sk += dS;
				F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0), F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] - (1 - b[ii]) * F[ii] + c[ii] * F[ii + 1]));
			}
			Sk += dS;
			F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0), F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] - (1 - b[ii]) * F[ii]));
			error = norm(F - F_tmp);
			F = F_tmp;
		}
		grid[0][jj - 1] = F0 * std::exp(-curve.integral(dT * (jj - 1)));
		for (zz = 1; zz < spot_mesh_; zz++) {
			grid[zz][jj - 1] = F[zz - 1];
		}
		grid[zz][jj - 1] = (FM - K_ * std::exp(-curve.integral(dT * (jj - 1)))) * (contract_type_ == 1);
	}
}

void Option::solve() {
	double Sk = 0;
	size_t ii = 0;
	F.resize(spot_mesh_ - 1);
	for (; ii <= spot_mesh_; ii++) {
		grid[ii][time_mesh_ - 1] = std::max(contract_type_ * (Sk - K_), 0.0);
		if (ii != 0 && ii != spot_mesh_) F[ii - 1] = (grid[ii][time_mesh_ - 1]);
		Sk += dS;
	}

	if (exercise_type_) {
		european_price();
	}
	else {
		american_price();
	}
}

double Option::price() {
	return grid[S0_ / dS][0];
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