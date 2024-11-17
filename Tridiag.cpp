#include "Tridiag.h"

#include<iostream>

Tridiag::Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag) {
	subdiag_ = subdiag;
	diag_ = diag;
	superdiag_ = superdiag;
}

std::vector<double> Tridiag::operator*(std::vector<double> x) {
	std::vector<double> b;
	b.push_back(diag_[0] * x[0] + superdiag_[0] * x[1]);

	size_t ii = 1;
	for (; ii < x.size() - 1; ii++) {
		b.push_back(subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii] + superdiag_[ii] * x[ii + 1]);
	}
	b.push_back(subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii]);
	return b;
}

std::vector<double> Tridiag::solve(std::vector<double> b) {
	std::vector<double> l, v;
	std::vector<double> c = superdiag_;

	v.push_back(diag_[0]);
	for (size_t ii = 0; ii < subdiag_.size(); ii++) {
		l.push_back(subdiag_[ii] / v[ii]);
		v.push_back(diag_[ii + 1] - l[ii] * c[ii]);
	}

	Lower L(l, std::vector<double>(diag_.size(), 1.0));
	Upper U(v, c);

	std::vector<double> y = L.solve(b);
	std::vector<double> x = U.solve(y);

	return x;
}