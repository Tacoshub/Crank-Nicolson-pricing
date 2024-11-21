/**
 * @file Tridiag.cpp
 * @brief Contains methods to handle tridiagonal matrices, including matrix-vector multiplication and solving linear systems.
 */

#include "Tridiag.h"

 /**
  * @brief Constructs a Tridiag object with specified subdiagonal, diagonal, and superdiagonal elements.
  * @param subdiag Vector containing the subdiagonal elements.
  * @param diag Vector containing the diagonal elements.
  * @param superdiag Vector containing the superdiagonal elements.
  */
Tridiag::Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag) {
    subdiag_ = subdiag;
    diag_ = diag;
    superdiag_ = superdiag;
}

/**
 * @brief Multiplies the tridiagonal matrix by a vector.
 *
 * This method computes the matrix-vector product \( b = A \cdot x \), where \( A \) is the tridiagonal matrix.
 * The matrix is represented by its subdiagonal, diagonal, and superdiagonal elements.
 *
 * @param x The vector to multiply.
 * @return Resulting vector \( b \) after multiplication.
 */
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

/**
 * @brief Solves the system of linear equations \( A \cdot x = b \), where \( A \) is a tridiagonal matrix.
 *
 * This method uses LU decomposition to split \( A \) into a lower triangular matrix \( L \) and an upper triangular matrix \( U \).
 * The solution is obtained in two steps:
 * 1. Forward substitution to solve \( L \cdot y = b \).
 * 2. Backward substitution to solve \( U \cdot x = y \).
 *
 * @param b Right-hand side vector of the system.
 * @return Solution vector \( x \).
 */
std::vector<double> Tridiag::solve(std::vector<double> b) {
    size_t n = b.size();

    std::vector<double> l(n - 1), v(n);
    std::vector<double> c = superdiag_;

    v[0] = diag_[0];
    for (size_t ii = 0; ii < subdiag_.size(); ii++) {
        l[ii] = subdiag_[ii] / v[ii];
        v[ii + 1] = diag_[ii + 1] - l[ii] * c[ii];
    }

    Lower L(l, std::vector<double>(diag_.size(), 1.0));
    Upper U(v, c);

    std::vector<double> y = L.solve(b);
    std::vector<double> x = U.solve(y);

    return x;
}