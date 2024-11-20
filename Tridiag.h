/**
 * @file MatrixDecomposition.h
 * @brief Contains the definitions of the Lower, Upper, and Tridiag classes for matrix decomposition and solving linear systems.
 *
 * This file defines classes to work with tridiagonal matrices, including methods for solving systems of linear equations
 * using forward and backward substitution. The `Tridiag` class also includes methods for matrix-vector multiplication
 * and displaying the matrix.
 */

#pragma once

#include <vector>
#include <iostream>
#include <iomanip>

 /**
  * @class Lower
  * @brief Represents the lower triangular part of a matrix and provides a method for solving linear systems.
  *
  * The `Lower` class models a lower triangular matrix (with non-zero entries on the diagonal and below it)
  * and provides a method to solve systems of linear equations using forward substitution.
  */
class Lower {

    std::vector<double> subdiag_; ///< Subdiagonal elements of the lower triangular matrix
    std::vector<double> diag_;    ///< Diagonal elements of the lower triangular matrix

public:

    /**
     * @brief Constructs a Lower object with the given subdiagonal and diagonal elements.
     *
     * @param subdiag A vector containing the subdiagonal elements.
     * @param diag A vector containing the diagonal elements.
     */
    Lower(std::vector<double> subdiag, std::vector<double> diag) : subdiag_(subdiag), diag_(diag) {}

    /**
     * @brief Solves a system of linear equations using forward substitution.
     *
     * This method solves the system `Lx = b`, where `L` is a lower triangular matrix and `b` is the right-hand side vector.
     * The solution is computed using forward substitution.
     *
     * @param b The right-hand side vector of the system of equations.
     * @return A vector containing the solution to the system of equations.
     */
    std::vector<double> solve(std::vector<double> b) {
        std::vector<double> x(b.size());
        x[0] = b[0] / diag_[0];
        for (size_t ii = 1; ii < b.size(); ii++) {
            x[ii] = (b[ii] - subdiag_[ii - 1] * x[ii - 1]) / diag_[ii];
        }
        return x;
    }
};

/**
 * @class Upper
 * @brief Represents the upper triangular part of a matrix and provides a method for solving linear systems.
 *
 * The `Upper` class models an upper triangular matrix (with non-zero entries on the diagonal and above it)
 * and provides a method to solve systems of linear equations using backward substitution.
 */
class Upper {

    std::vector<double> diag_;      ///< Diagonal elements of the upper triangular matrix
    std::vector<double> superdiag_; ///< Superdiagonal elements of the upper triangular matrix

public:

    /**
     * @brief Constructs an Upper object with the given diagonal and superdiagonal elements.
     *
     * @param diag A vector containing the diagonal elements.
     * @param superdiag A vector containing the superdiagonal elements.
     */
    Upper(std::vector<double> diag, std::vector<double> superdiag) : diag_(diag), superdiag_(superdiag) {}

    /**
     * @brief Solves a system of linear equations using backward substitution.
     *
     * This method solves the system `Ux = b`, where `U` is an upper triangular matrix and `b` is the right-hand side vector.
     * The solution is computed using backward substitution.
     *
     * @param b The right-hand side vector of the system of equations.
     * @return A vector containing the solution to the system of equations.
     */
    std::vector<double> solve(std::vector<double> b) {
        int n = b.size();
        std::vector<double> x(n);
        x[n - 1] = b[n - 1] / diag_[n - 1];
        for (int ii = n - 2; ii >= 0; ii--) {
            x[ii] = (b[ii] - superdiag_[ii] * x[ii + 1]) / diag_[ii];
        }
        return x;
    }
};

/**
 * @class Tridiag
 * @brief Represents a tridiagonal matrix and provides methods for matrix-vector multiplication and solving linear systems.
 *
 * The `Tridiag` class models a tridiagonal matrix (with non-zero elements on the diagonal, subdiagonal, and superdiagonal)
 * and provides functions to multiply the matrix by a vector and solve systems of linear equations using LU decomposition.
 */
class Tridiag {

    std::vector<double> subdiag_;    ///< Subdiagonal elements of the tridiagonal matrix
    std::vector<double> diag_;       ///< Diagonal elements of the tridiagonal matrix
    std::vector<double> superdiag_;  ///< Superdiagonal elements of the tridiagonal matrix

public:

    /**
     * @brief Default constructor for the Tridiag class.
     */
    Tridiag() {}

    /**
     * @brief Constructs a Tridiag object with the given subdiagonal, diagonal, and superdiagonal elements.
     *
     * @param subdiag A vector containing the subdiagonal elements.
     * @param diag A vector containing the diagonal elements.
     * @param superdiag A vector containing the superdiagonal elements.
     */
    Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag);

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
    std::vector<double> operator*(std::vector<double> x);

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
    std::vector<double> solve(std::vector<double> b);

    /**
     * @brief Returns the size of the matrix (number of rows/columns).
     *
     * @return The size of the matrix (i.e., the number of rows or columns).
     */
    size_t size() { return diag_.size(); }

    /**
     * @brief Displays the tridiagonal matrix in a readable format.
     *
     * This function prints the tridiagonal matrix to the console, with zero elements displayed as "0.00"
     * and non-zero elements formatted to 2 decimal places.
     */
    void display() {
        size_t n = diag_.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) {
                    std::cout << std::setw(8) << diag_[i] << " ";
                }
                else if (i == j + 1) {
                    std::cout << std::setw(8) << subdiag_[j] << " ";
                }
                else if (j == i + 1) {
                    std::cout << std::setw(8) << superdiag_[i] << " ";
                }
                else {
                    std::cout << std::setw(8) << "0.00" << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};
