#include "Tridiag.h"

Tridiag::Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag) {
	subdiag_ = subdiag;
	diag_ = diag;
	superdiag_ = superdiag;
}

std::vector<double> Tridiag::operator*(std::vector<double> x) {
	size_t n = x.size();
	std::vector<double> b(n);
	b[0] = (diag_[0] * x[0] + superdiag_[0] * x[1]);

	size_t ii = 1;
	for (; ii < n - 1; ii++) {
		b[ii] = (subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii] + superdiag_[ii] * x[ii + 1]);
	}
	b[ii] = (subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii]);
	return b;
}

std::vector<double> Tridiag::solve(std::vector<double> b) {
	size_t n = b.size();
	std::vector<double> l(n - 1), v(n);
	std::vector<double> c = superdiag_;

	v[0] = (diag_[0]);
	for (size_t ii = 0; ii < subdiag_.size(); ii++) {
		l[ii] = (subdiag_[ii] / v[ii]);
		v[ii + 1] = (diag_[ii + 1] - l[ii] * c[ii]);
	}

	Lower L(l, std::vector<double>(diag_.size(), 1.0));
	Upper U(v, c);

	std::vector<double> y = L.solve(b);
	std::vector<double> x = U.solve(y);

	return x;
}