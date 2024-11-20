/**
 * @file Tridiag.cpp
 * @brief Contains the implementation of the Tridiag class for working with tridiagonal matrices.
 *
 * This file implements the `Tridiag` class, which represents a tridiagonal matrix and provides methods
 * for multiplying the matrix by a vector and solving a system of linear equations using the LU decomposition.
 */

#include "Tridiag.h"

 /**
  * @class Tridiag
  * @brief Represents a tridiagonal matrix and provides methods for matrix-vector multiplication and solving linear systems.
  *
  * The `Tridiag` class models a tridiagonal matrix and provides functions to multiply the matrix with a vector,
  * as well as to solve systems of linear equations involving the matrix using LU decomposition.
  */
Tridiag::Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag) {
    subdiag_ = subdiag;
    diag_ = diag;
    superdiag_ = superdiag;
}

/**
 * @brief Multiplies the tridiagonal matrix by a vector.
 *
 * This function multiplies the tridiagonal matrix (represented by the `Tridiag` class) with a given vector `x`
 * and returns the result as a new vector. The matrix-vector multiplication is done in a computationally efficient
 * manner by taking advantage of the sparsity of the tridiagonal matrix.
 *
 * @param x The vector to be multiplied by the matrix.
 * @return A vector resulting from the matrix-vector multiplication.
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
 * @brief Solves a system of linear equations involving the tridiagonal matrix.
 *
 * This function solves a system of linear equations of the form `Ax = b`, where `A` is a tridiagonal matrix
 * and `b` is the given vector. The solution is computed using LU decomposition, where the matrix `A` is decomposed
 * into a lower triangular matrix `L` and an upper triangular matrix `U`. The function first solves for `y` using `L`
 * and then solves for `x` using `U`.
 *
 * @param b The right-hand side vector of the system of equations.
 * @return The solution vector `x` to the system of equations `Ax = b`.
 */
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
