/**
 * @file Tridiag.h
 * @brief Contains methods to handle tridiagonal matrices, including matrix-vector multiplication and solving linear systems.
 */

#pragma once

#include <vector>
#include <iostream>
#include <iomanip>

/**
 * @class Lower
 * @brief Represents a lower triangular matrix and provides functionality to solve linear equations.
 */
class Lower {

    std::vector<double> subdiag_;
    std::vector<double> diag_;

public:
    /**
     * @brief Constructs a Lower object with specified subdiagonal and diagonal elements.
     * @param subdiag Vector containing subdiagonal elements.
     * @param diag Vector containing diagonal elements.
     */
    Lower(std::vector<double> subdiag, std::vector<double> diag) : subdiag_(subdiag), diag_(diag) {}

    /**
     * @brief Solves a system of equations for the lower triangular matrix.
     * @param b The right-hand side vector.
     * @return Solution vector.
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
 * @brief Represents an upper triangular matrix and provides functionality to solve linear equations.
 */
class Upper {

    std::vector<double> diag_;
    std::vector<double> superdiag_;

public:
    /**
     * @brief Constructs an Upper object with specified diagonal and superdiagonal elements.
     * @param diag Vector containing diagonal elements.
     * @param superdiag Vector containing superdiagonal elements.
     */
    Upper(std::vector<double> diag, std::vector<double> superdiag) : diag_(diag), superdiag_(superdiag) {}

    /**
     * @brief Solves a system of equations for the upper triangular matrix.
     * @param b The right-hand side vector.
     * @return Solution vector.
     */
    std::vector<double> solve(std::vector<double> b) {
        int n = int(b.size());
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
 * @brief Represents a tridiagonal matrix and provides functionality for matrix operations and solving equations.
 */
class Tridiag {

    std::vector<double> subdiag_;
    std::vector<double> diag_;
    std::vector<double> superdiag_;

public:
    /**
     * @brief Default constructor for an empty tridiagonal matrix.
     */
    Tridiag() {}

    /**
     * @brief Constructs a Tridiag object with specified subdiagonal, diagonal, and superdiagonal elements.
     * @param subdiag Vector containing subdiagonal elements.
     * @param diag Vector containing diagonal elements.
     * @param superdiag Vector containing superdiagonal elements.
     */
    Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag);

    /**
     * @brief Multiplies the tridiagonal matrix by a vector.
     * @param x Input vector.
     * @return Resulting vector after multiplication.
     */
    std::vector<double> operator*(std::vector<double> x);

    /**
     * @brief Solves a system of equations for the tridiagonal matrix.
     * @param b The right-hand side vector.
     * @return Solution vector.
     */
    std::vector<double> solve(std::vector<double> b);

    /**
     * @brief Returns the size of the tridiagonal matrix.
     * @return The number of rows or columns in the matrix.
     */
    size_t size() { return diag_.size(); }

    /**
     * @brief Displays the tridiagonal matrix in a formatted way.
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