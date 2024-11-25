/**
 * @file Option.h
 * @brief Class to create and manage an Option object with numerical pricing methods.
 */

#pragma once

#include "InterestRate.h"
#include "OptionExceptions.h"
#include "Tridiag.h"

#include <vector>

 /**
  * @class Option
  * @brief Represents an option contract with numerical pricing methods using finite difference techniques.
  */
class Option {
    int contract_type_;
    int exercise_type_;
    double T_;
    double K_;
    double T0_;
    double S0_;
    double volatility_;
    unsigned int time_mesh_;
    unsigned int spot_mesh_;
    std::vector<std::pair<double, double>> interest_rate_;
    InterestRate curve;
    double dT;
    double dS;
    double F0;
    double FM;
    std::vector<std::vector<double>> grid;
    std::vector<double> F;
    double tol_;
    double w_;

    void create_grid();
    void european_price();
    void american_price();

public:
    /**
     * @brief Constructs an Option object.
     * @param contract_type Type of contract (1 for Call, -1 for Put).
     * @param exercise_type Exercise type (0 for European, 1 for American).
     * @param T Maturity of the option.
     * @param K Strike price.
     * @param T0 Initial time.
     * @param time_mesh Number of time steps in the grid.
     * @param spot_mesh Number of spot steps in the grid.
     * @param S0 Initial spot price.
     * @param interest_rate Interest rate curve as pairs (time, rate).
     * @param volatility Volatility of the underlying asset.
     * @param tol Convergence tolerance for iterative solvers.
     * @param w Relaxation parameter for iterative solvers.
     */
    Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol = 0.01, double w = 1.2);

    /**
     * @brief Computes the coefficients a_j for the tridiagonal matrix.
     * @param i Time step index.
     * @return Vector of coefficients a_j.
     */
    std::vector<double> compute_aj(size_t i);

    /**
     * @brief Computes the coefficients b_j for the tridiagonal matrix.
     * @param i Time step index.
     * @return Vector of coefficients b_j.
     */
    std::vector<double> compute_bj(size_t i);

    /**
     * @brief Computes the coefficients c_j for the tridiagonal matrix.
     * @param i Time step index.
     * @return Vector of coefficients c_j.
     */
    std::vector<double> compute_cj(size_t i);

    /**
     * @brief Computes the tridiagonal matrix C.
     * @param a Vector of coefficients a_j.
     * @param b Vector of coefficients b_j.
     * @param c Vector of coefficients c_j.
     * @return Tridiagonal matrix C.
     */
    Tridiag compute_C(std::vector<double> a, std::vector<double> b, std::vector<double> c);

    /**
     * @brief Computes the tridiagonal matrix D.
     * @param a Vector of coefficients a_j.
     * @param b Vector of coefficients b_j.
     * @param c Vector of coefficients c_j.
     * @return Tridiagonal matrix D.
     */
    Tridiag compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c);

    /**
     * @brief Computes the vector K for the boundary conditions.
     * @param i Time step index.
     * @return Pair of boundary values for the vector K.
     */
    std::pair<double, double> compute_K(size_t i);

    /**
     * @brief Computes the boundary condition K for American options.
     * @param i Time step index.
     * @return Boundary value for K.
     */
    double compute_K_american(size_t i);

    /**
     * @brief Solves the option pricing problem using the grid.
     */
    void solve();

    /**
     * @brief Displays the computational grid for debugging purposes.
     */
    void display_grid();

    /**
     * @brief Computes and returns the option price.
     * @return Price of the option.
     */
    double price();

    /**
     * @brief Computes the delta of the option.
     * @param h Step size for finite difference.
     * @return Delta of the option.
     */
    double delta(double h);

    /**
     * @brief Computes the gamma of the option.
     * @param h Step size for finite difference.
     * @return Gamma of the option.
     */
    double gamma(double h);

    /**
     * @brief Computes the theta of the option.
     * @param h Step size for finite difference.
     * @return Theta of the option.
     */
    double theta(double h);

    /**
     * @brief Computes the vega of the option.
     * @param h Step size for finite difference.
     * @return Vega of the option.
     */
    double vega(double h);

    /**
     * @brief Computes the rho of the option.
     * @param h Step size for finite difference.
     * @return Rho of the option.
     */
    double rho(double h);
};