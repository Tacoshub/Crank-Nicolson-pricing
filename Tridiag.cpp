#include "Tridiag.h"

/**
 * @brief Constructs a Tridiag object by initializing the subdiagonal, diagonal, and superdiagonal elements.
 * @param subdiag Vector containing subdiagonal elements.
 * @param diag Vector containing diagonal elements.
 * @param superdiag Vector containing superdiagonal elements.
 */
Tridiag::Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag) {
    subdiag_ = subdiag;
    diag_ = diag;
    superdiag_ = superdiag;
}

/**
 * @brief Multiplies the tridiagonal matrix by a vector.
 *
 * Computes the matrix-vector product \( b = A \cdot x \), where \( A \) is the tridiagonal matrix.
 *
 * @param x Input vector to multiply.
 * @return Resulting vector \( b \) after multiplication.
 */
std::vector<double> Tridiag::operator*(std::vector<double> x) {
    size_t n = x.size();
    std::vector<double> b(n);

    // First row computation
    b[0] = (diag_[0] * x[0] + superdiag_[0] * x[1]);

    // Middle rows computation
    size_t ii = 1;
    for (; ii < n - 1; ii++) {
        b[ii] = (subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii] + superdiag_[ii] * x[ii + 1]);
    }

    // Last row computation
    b[ii] = (subdiag_[ii - 1] * x[ii - 1] + diag_[ii] * x[ii]);

    return b;
}

/**
 * @brief Solves the system of linear equations \( A \cdot x = b \), where \( A \) is the tridiagonal matrix.
 *
 * This method uses LU decomposition to split the tridiagonal matrix into a lower triangular matrix \( L \)
 * and an upper triangular matrix \( U \). It then solves the system in two steps:
 * 1. Forward substitution to solve \( L \cdot y = b \).
 * 2. Backward substitution to solve \( U \cdot x = y \).
 *
 * @param b Right-hand side vector.
 * @return Solution vector \( x \).
 */
std::vector<double> Tridiag::solve(std::vector<double> b) {
    size_t n = b.size();

    // Temporary vectors for LU decomposition
    std::vector<double> l(n - 1), v(n);
    std::vector<double> c = superdiag_;

    // Compute diagonal elements of U and subdiagonal elements of L
    v[0] = diag_[0];
    for (size_t ii = 0; ii < subdiag_.size(); ii++) {
        l[ii] = subdiag_[ii] / v[ii];
        v[ii + 1] = diag_[ii + 1] - l[ii] * c[ii];
    }

    // Create L and U matrices
    Lower L(l, std::vector<double>(diag_.size(), 1.0)); ///< Lower triangular matrix with unit diagonal.
    Upper U(v, c); ///< Upper triangular matrix.

    // Solve L * y = b using forward substitution
    std::vector<double> y = L.solve(b);

    // Solve U * x = y using backward substitution
    std::vector<double> x = U.solve(y);

    return x;
}