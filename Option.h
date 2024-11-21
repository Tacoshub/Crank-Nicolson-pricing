/**
 * @file Option.h
 * @brief class to create an Option object
 */
#pragma once

#include "InterestRate.h"
#include "OptionExceptions.h"
#include "Tridiag.h"

#include <vector>

/**
 * @class Option
 * @brief Represents an option contract with numerical pricing methods.
 */
class Option {

    int contract_type_; ///< Type of the contract (e.g., 0 for call, 1 for put).
    bool exercise_type_; ///< Exercise type (e.g., false for European, true for American).
    double T_; ///< Maturity of the option.
    double K_; ///< Strike price of the option.
    double T0_; ///< Initial time of the contract.
    double S0_; ///< Initial spot price.
    double volatility_; ///< Volatility of the underlying asset.
    unsigned int time_mesh_; ///< Number of time steps in the grid.
    unsigned int spot_mesh_; ///< Number of spot steps in the grid.

    std::vector<std::pair<double, double>> interest_rate_; ///< Interest rate curve as pairs (time, rate).
    InterestRate curve; ///< Interpolated interest rate curve.

    double dT; ///< Time step size.
    double dS; ///< Spot step size.
    double F0; ///< Boundary condition for the minimum spot value.
    double FM; ///< Boundary condition for the maximum spot value.
    std::vector<std::vector<double>> grid; ///< Grid for option prices.
    std::vector<double> F; ///< Temporary vector for computation.

    /**
     * @brief Creates the computational grid for pricing the option.
     */
    void create_grid();

    /**
     * @brief Computes the price for a European option.
     */
    void european_price();

    /**
     * @brief Computes the price for an American option.
     */
    void american_price();

    double tol_; ///< Convergence tolerance for iterative solvers.
    double w_; ///< Relaxation parameter for iterative solvers.

public:

    /**
     * @brief Constructs an Option object.
     *
     * @param contract_type Type of contract (0 for call, 1 for put).
     * @param exercise_type Exercise type (0 for European, 1 for American).
     * @param T Maturity of the option.
     * @param K Strike price.
     * @param T0 Initial time.
     * @param time_mesh Number of time steps in the grid.
     * @param spot_mesh Number of spot steps in the grid.
     * @param S0 Initial spot price.
     * @param interest_rate Interest rate curve as pairs (time, rate).
     * @param volatility Volatility of the underlying asset.
     * @param tol Convergence tolerance (default is 0.01).
     * @param w Relaxation parameter (default is 1.2).
     */
    Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol = 0.01, double w = 1.2);

    /**
     * @brief Computes the coefficients a_j for the tridiagonal matrix.
     *
     * @param i Time step index.
     * @return Vector of coefficients a_j.
     */
    std::vector<double> compute_aj(size_t i);

    /**
     * @brief Computes the coefficients b_j for the tridiagonal matrix.
     *
     * @param i Time step index.
     * @return Vector of coefficients b_j.
     */
    std::vector<double> compute_bj(size_t i);

    /**
     * @brief Computes the coefficients c_j for the tridiagonal matrix.
     *
     * @param i Time step index.
     * @return Vector of coefficients c_j.
     */
    std::vector<double> compute_cj(size_t i);

    /**
     * @brief Computes the tridiagonal matrix C.
     *
     * @param a Vector of coefficients a_j.
     * @param b Vector of coefficients b_j.
     * @param c Vector of coefficients c_j.
     * @return Tridiagonal matrix C.
     */
    Tridiag compute_C(std::vector<double> a, std::vector<double> b, std::vector<double> c);

    /**
     * @brief Computes the tridiagonal matrix D.
     *
     * @param a Vector of coefficients a_j.
     * @param b Vector of coefficients b_j.
     * @param c Vector of coefficients c_j.
     * @return Tridiagonal matrix D.
     */
    Tridiag compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c);

    /**
     * @brief Computes the vector K for the boundary conditions.
     *
     * @param i Time step index.
     * @return Pair of boundary values for the vector K.
     */
    std::pair<double, double> compute_K(unsigned int i);

    /**
     * @brief Computes the boundary condition K for American options.
     *
     * @param i Time step index.
     * @return Boundary value for K.
     */
    double compute_K_american(unsigned int i);

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
     *
     * @return Price of the option.
     */
    double price();

    /**
     * @brief Computes the delta of the option.
     *
     * @param h Step size for finite difference.
     * @return Delta of the option.
     */
    double delta(double h);

    /**
     * @brief Computes the gamma of the option.
     *
     * @param h Step size for finite difference.
     * @return Gamma of the option.
     */
    double gamma(double h);

    /**
     * @brief Computes the theta of the option.
     *
     * @param h Step size for finite difference.
     * @return Theta of the option.
     */
    double theta(double h);

    /**
     * @brief Computes the vega of the option.
     *
     * @param h Step size for finite difference.
     * @return Vega of the option.
     */
    double vega(double h);

    /**
     * @brief Computes the rho of the option.
     *
     * @param h Step size for finite difference.
     * @return Rho of the option.
     */
    double rho(double h);

    /**
    * @brief Generates a plot of the option price as a function of the spot price.
    *
    * This function computes a grid of option prices and extracts the values
    * for a fixed time slice to create a plot of option prices against spot prices.
    *
    * @return A vector of pairs, where each pair contains a spot price and the corresponding option price.
    */
    std::vector<std::pair<double, double>> price_plot();

    /**
     * @brief Generates a plot of the option's delta as a function of the spot price.
     *
     * This function computes the delta (rate of change of the option price with respect
     * to the spot price) at various spot prices and stores the results in a vector.
     *
     * @param h A small perturbation used for numerical differentiation.
     * @return A vector of pairs, where each pair contains a spot price and the corresponding delta.
     */
    std::vector<std::pair<double, double>> delta_plot(double h);

    /**
     * @brief Computes the exercise boundary for the option.
     *
     * This function determines the boundary separating the exercise and hold regions
     * for the option over time. The boundary is identified by finding the spot price
     * at each time step where the difference between the option value and the payoff is minimal.
     *
     * @return A vector of pairs, where each pair contains a time step and the corresponding exercise boundary spot price.
     */
    std::vector<std::pair<double, double>> exercise_boundary();

};
