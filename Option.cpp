/**
 * @file Option.cpp
 * @brief Contains various methods to deal with the Option object, including pricing and Greeks computation.
 */

#include "Option.h"
#include "Tridiag.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

 /**
  * @brief Multiplies a scalar with a vector.
  *
  * Each element of the vector is multiplied by the scalar \( k \).
  *
  * @param k Scalar value.
  * @param v Vector to be scaled.
  * @return Scaled vector.
  */
std::vector<double> operator*(double k, std::vector<double> v) {
    size_t n = v.size();
    for (size_t ii = 0; ii < n; ii++) {
        v[ii] *= k;
    }
    return v;
}

/**
 * @brief Subtracts a vector from a scalar.
 *
 * Each element of the vector is subtracted from the scalar \( k \).
 *
 * @param k Scalar value.
 * @param v Vector to subtract from the scalar.
 * @return Resulting vector.
 */
std::vector<double> operator-(double k, std::vector<double> v) {
    size_t n = v.size();
    for (size_t ii = 0; ii < n; ii++) {
        v[ii] = k - v[ii];
    }
    return v;
}

/**
 * @brief Adds a scalar to a vector.
 *
 * Adds \( k \) to each element of the vector.
 *
 * @param k Scalar value.
 * @param v Vector to which the scalar is added.
 * @return Resulting vector.
 */
std::vector<double> operator+(double k, std::vector<double> v) {
    size_t n = v.size();
    for (size_t ii = 0; ii < n; ii++) {
        v[ii] = k + v[ii];
    }
    return v;
}

/**
 * @brief Subtracts two vectors element-wise.
 *
 * Computes the difference between corresponding elements of two vectors.
 *
 * @param v1 First vector.
 * @param v2 Second vector to subtract.
 * @return Resulting vector.
 */
std::vector<double> operator-(std::vector<double> v1, std::vector<double> v2) {
    size_t n = v1.size();
    std::vector<double> res(n);
    for (size_t ii = 0; ii < n; ii++) {
        res[ii] = v1[ii] - v2[ii];
    }
    return res;
}

/**
 * @brief Adds a scalar to the first element of a vector.
 *
 * The scalar \( p2 \) is added only to the first element of the vector.
 *
 * @param v1 Vector to modify.
 * @param p2 Scalar to add.
 * @return Modified vector.
 */
std::vector<double> operator+(std::vector<double> v1, double p2) {
    v1[0] += p2;
    return v1;
}

/**
 * @brief Adds a pair of scalars to the first and last elements of a vector.
 *
 * Adds the first element of the pair to the first vector element, and the second
 * element of the pair to the last vector element.
 *
 * @param v1 Vector to modify.
 * @param p2 Pair of scalars to add.
 * @return Modified vector.
 */
std::vector<double> operator+(std::vector<double> v1, std::pair<double, double> p2) {
    v1[0] += p2.first;
    v1[v1.size() - 1] += p2.second;
    return v1;
}

/**
 * @brief Computes the Euclidean norm of a vector.
 *
 * The norm is calculated as:
 * \[
 * \text{norm} = \sqrt{\sum_i v_i^2}
 * \]
 *
 * @param v1 Input vector.
 * @return Euclidean norm.
 */
double norm(std::vector<double> v1) {
    size_t n = v1.size();
    double som = 0;
    for (size_t ii = 0; ii < n; ii++) {
        som += v1[ii] * v1[ii];
    }
    return std::sqrt(som);
}

/**
 * @brief Constructs an Option object and validates input parameters.
 *
 * Initializes the option with specified contract type, exercise type, maturity, strike price,
 * and other parameters required for the finite difference method.
 *
 * @param contract_type Type of option: 1 for Call, -1 for Put.
 * @param exercise_type Exercise type: 1 for European, 0 for American.
 * @param T Maturity time.
 * @param K Strike price.
 * @param T0 Start time.
 * @param time_mesh Number of time steps.
 * @param spot_mesh Number of spot price steps.
 * @param S0 Current spot price.
 * @param interest_rate Interest rate curve as a vector of (time, rate) pairs.
 * @param volatility Volatility of the underlying asset.
 * @param tol Tolerance for iterative methods.
 * @param w Relaxation parameter for iterative methods.
 */
Option::Option(int contract_type, int exercise_type, double T, double K, double T0, unsigned int time_mesh, unsigned int spot_mesh, double S0, std::vector<std::pair<double, double>> interest_rate, double volatility, double tol, double w)
    : contract_type_(contract_type), exercise_type_(exercise_type), T_(T), K_(K), T0_(T0),
    time_mesh_(time_mesh), spot_mesh_(spot_mesh), S0_(S0), interest_rate_(interest_rate),
    volatility_(volatility), tol_(tol), w_(w) {
    if (contract_type_ != 1 && contract_type_ != -1) throw InvalidContractType(contract_type_);
    if (exercise_type_ != 1 && exercise_type_ != 0) throw InvalidExerciseType(exercise_type_);
    if (T_ < T0 || T < 0) throw InvalidMaturity();
    if (K_ <= 0) throw InvalidStrike(K_);
    if (time_mesh_ <= 0) throw InvalidTimeMesh(time_mesh_);
    if (spot_mesh_ <= 0) throw InvalidSpotMesh(spot_mesh_);
    if (S0_ <= 0) throw InvalidSpot(S0_);
    if (volatility_ <= 0) throw InvalidVolatility(volatility_);

    dT = (T_ - T0_) / time_mesh_;
    dS = (5 * S0_) / spot_mesh_;

    curve = InterestRate(interest_rate_);

    if (contract_type == 1) { F0 = 0, FM = 5 * S0; }
    else { F0 = K, FM = 0; }

    create_grid();
    solve();
}

/**
 * @brief Creates the grid for option pricing.
 *
 * Initializes a 2D grid of size `(spot_mesh_ + 1) x time_mesh_` with zeros
 * to store intermediate and final option values during the finite difference computation.
 */
void Option::create_grid() {
    std::vector<std::vector<double>> matrix(spot_mesh_ + 1, std::vector<double>(time_mesh_, 0.0));
    grid = matrix;
}

/**
 * @brief Computes coefficients \( a_j \) for the tridiagonal matrix in the finite difference method.
 *
 * Coefficients \( a_j \) are used for the subdiagonal elements in the tridiagonal matrix and
 * depend on the volatility and interest rate at the current time step.
 *
 * @param i Time step index.
 * @return Vector of coefficients \( a_j \).
 */
std::vector<double> Option::compute_aj(size_t i) {
    std::vector<double> aj(spot_mesh_ - 2);
    for (size_t jj = 2; jj < spot_mesh_; jj++) {
        aj[jj - 2] = (dT / 4) * (volatility_ * volatility_ * jj * jj - curve(dT * i) * jj);
    }
    return aj;
}

/**
 * @brief Computes coefficients \( b_j \) for the tridiagonal matrix in the finite difference method.
 *
 * Coefficients \( b_j \) are used for the diagonal elements in the tridiagonal matrix
 * and depend on the volatility and interest rate at the current time step.
 *
 * @param i Time step index.
 * @return Vector of coefficients \( b_j \).
 */
std::vector<double> Option::compute_bj(size_t i) {
    std::vector<double> bj(spot_mesh_ - 1);
    for (size_t jj = 1; jj < spot_mesh_; jj++) {
        bj[jj - 1] = -(dT / 2) * (volatility_ * volatility_ * jj * jj + curve(dT * i));
    }
    return bj;
}

/**
 * @brief Computes coefficients \( c_j \) for the tridiagonal matrix in the finite difference method.
 *
 * Coefficients \( c_j \) are used for the superdiagonal elements in the tridiagonal matrix and
 * depend on the volatility and interest rate at the current time step.
 *
 * @param i Time step index.
 * @return Vector of coefficients \( c_j \).
 */
std::vector<double> Option::compute_cj(size_t i) {
    std::vector<double> cj(spot_mesh_ - 2);
    for (size_t jj = 1; jj < spot_mesh_ - 1; jj++) {
        cj[jj - 1] = (dT / 4) * (volatility_ * volatility_ * jj * jj + curve(dT * i) * jj);
    }
    return cj;
}

/**
 * @brief Constructs the tridiagonal matrix \( C \) used in the finite difference method.
 *
 * The matrix \( C \) is defined as:
 * \[
 * C = \text{Tridiag}(-a, 1 - b, -c)
 * \]
 * where \( a, b, c \) are vectors of coefficients.
 *
 * @param a Vector of subdiagonal coefficients \( a_j \).
 * @param b Vector of diagonal coefficients \( b_j \).
 * @param c Vector of superdiagonal coefficients \( c_j \).
 * @return Tridiag object representing the matrix \( C \).
 */
Tridiag Option::compute_C(std::vector<double> a, std::vector<double> b, std::vector<double> c) {
    return Tridiag(-1.0 * a, 1.0 - b, -1.0 * c);
}

/**
 * @brief Constructs the tridiagonal matrix \( D \) used in the finite difference method.
 *
 * The matrix \( D \) is defined as:
 * \[
 * D = \text{Tridiag}(a, 1 + b, c)
 * \]
 * where \( a, b, c \) are vectors of coefficients.
 *
 * @param a Vector of subdiagonal coefficients \( a_j \).
 * @param b Vector of diagonal coefficients \( b_j \).
 * @param c Vector of superdiagonal coefficients \( c_j \).
 * @return Tridiag object representing the matrix \( D \).
 */
Tridiag Option::compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c) {
    return Tridiag(a, 1.0 + b, c);
}

/**
 * @brief Computes the boundary terms \( K_1 \) and \( K_2 \) used for pricing adjustments at the boundaries.
 *
 * These terms represent the contributions of the boundary conditions to the finite difference system.
 *
 * @param i Time step index.
 * @return A pair of boundary terms \( (K_1, K_2) \).
 */
std::pair<double, double> Option::compute_K(size_t i) {
    double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);
    double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);
    double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) + a1_curr * F0 * std::exp(-curve.integral(dT * i));

    double cm_prec = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * (i - 1)) * (spot_mesh_ - 1));
    double cm_curr = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * i) * (spot_mesh_ - 1));
    double K2 = cm_prec * (FM - K_ * std::exp(-curve.integral(dT * (i - 1)))) +
        cm_curr * (FM - K_ * std::exp(-curve.integral(dT * i)));

    return std::make_pair(K1, K2);
}

/**
 * @brief Computes the boundary term \( K_1 \) for American option pricing.
 *
 * The boundary term is specific to American options and accounts for the early exercise condition
 * at the lower boundary of the grid. It is computed using coefficients derived from the volatility,
 * interest rate, and boundary values.
 *
 * @param i The time step index.
 * @return The computed boundary term \( K_1 \).
 */
double Option::compute_K_american(size_t i) {
    double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);
    double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);
    double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) +
        a1_curr * F0 * std::exp(-curve.integral(dT * i));

    return K1;
}

/**
 * @brief Computes the price of a European option using the finite difference method.
 *
 * This method iterates backward in time over the pricing grid:
 * - At each time step, it constructs the tridiagonal matrices \( C \) and \( D \) using coefficients \( a_j, b_j, c_j \).
 * - Computes the right-hand side (RHS) of the linear system using \( D \) and boundary terms \( K \).
 * - Solves the linear system \( C \cdot F = \text{RHS} \) to update the option values.
 * - Applies boundary conditions for the grid values at each step.
 */
void Option::european_price() {
    Tridiag C, D;
    std::pair<double, double> K;
    std::vector<double> a, b, c, RHS;
    size_t zz;

    for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
        a = compute_aj(jj);
        b = compute_bj(jj);
        c = compute_cj(jj);

        C = compute_C(a, b, c);
        D = compute_D(a, b, c);
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

/**
 * @brief Computes the price of an American option using the finite difference method with a penalty approach.
 *
 * This method uses an iterative process to enforce the early exercise condition:
 * - Constructs the tridiagonal matrix \( D \) and computes the RHS.
 * - Iteratively solves the system with a penalty term to ensure the option value respects the exercise condition.
 * - Updates the option value grid by taking the maximum of the intrinsic value and the continuation value.
 * - Applies boundary conditions at each time step.
 */
void Option::american_price() {
    std::vector<double> a, b, c;
    Tridiag D;
    std::pair<double, double> K;
    double Sk;
    std::vector<double> RHS;
    size_t ii, zz;

    for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
        a = compute_aj(jj);
        b = compute_bj(jj);
        c = compute_cj(jj);

        D = compute_D(a, b, c);
        K = compute_K(jj);

        RHS = D * F + K;

        double error = 1e6;
        std::vector<double> F_tmp(spot_mesh_ - 1);

        while (error > tol_) {
            Sk = dS;
            F_tmp[0] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
                F[0] + (w_ / (1 - b[0])) * (RHS[0] - (1 - b[0]) * F[0] + c[0] * F[1]));

            ii = 1;
            for (; ii < spot_mesh_ - 2; ii++) {
                Sk += dS;
                F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
                    F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] -
                        (1 - b[ii]) * F[ii] + c[ii] * F[ii + 1]));
            }

            Sk += dS;
            F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
                F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] - (1 - b[ii]) * F[ii]));

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

/**
 * @brief Solves the option pricing problem.
 *
 * Initializes the pricing grid and determines the option price at earlier time steps
 * using either the `european_price` or `american_price` method.
 */
void Option::solve() {
    double Sk = 0;
    size_t ii = 0;

    F.resize(spot_mesh_ - 1);

    for (; ii <= spot_mesh_; ii++) {
        grid[ii][time_mesh_ - 1] = std::max(contract_type_ * (Sk - K_), 0.0);
        if (ii != 0 && ii != spot_mesh_) {
            F[ii - 1] = grid[ii][time_mesh_ - 1];
        }
        Sk += dS;
    }

    if (exercise_type_) {
        european_price();
    }
    else {
        american_price();
    }
}

/**
 * @brief Computes the price of the option using the finite difference method.
 *
 * Initializes the time and spot price step sizes, creates the grid, and solves the problem.
 *
 * @return The computed option price at \( S_0 \) and \( T_0 \).
 */
double Option::price() {
    return grid[S0_ / dS][0];
}

/**
 * @brief Displays the grid values for the option price.
 *
 * Outputs the values of the grid to the console in a tabular format,
 * with each row corresponding to a spot price and each column to a time step.
 */
void Option::display_grid() {
    std::cout << std::fixed << std::setprecision(3);
    for (size_t ii = 0; ii <= spot_mesh_; ii++) {
        for (size_t jj = 0; jj < time_mesh_; jj++) {
            std::cout << std::setw(7) << grid[ii][jj] << " ";
        }
        std::cout << '\n';
    }
}

/**
 * @brief Computes the Delta of the option.
 *
 * Delta measures the sensitivity of the option price to changes in the underlying asset price \( S_0 \).
 * It is computed using finite differences as:
 * \[
 * \Delta = \frac{\text{price}(S_0 + h) - \text{price}(S_0)}{h}
 * \]
 *
 * @param h Small increment for the underlying asset price.
 * @return The computed Delta value.
 */
double Option::delta(double S) {

    double d1 = grid[S / dS + 1][0];
    double d2 = grid[S / dS - 1][0];

    return (d1 - d2) / (2*dS);
}

/**
 * @brief Computes the Gamma of the option.
 *
 * Gamma measures the sensitivity of Delta to changes in the underlying asset price \( S_0 \).
 * It is computed using finite differences as:
 * \[
 * \Gamma = \frac{\Delta(S_0 + h) - \Delta(S_0)}{h}
 * \]
 *
 * @param h Small increment for the underlying asset price.
 * @return The computed Gamma value.
 */
double Option::gamma() {
    double g1 = grid[S0_ / dS + 1][0];
    double g2 = grid[S0_ / dS - 1][0];
    double g3 = grid[S0_ / dS][0];

    return (g1 + g2 - 2 * g3) / dS / dS;
}

/**
 * @brief Computes the Theta of the option.
 *
 * Theta measures the sensitivity of the option price to changes in time to maturity \( T \).
 * It is computed using finite differences as:
 * \[
 * \Theta = \frac{\text{price}(T - h) - \text{price}(T)}{h}
 * \]
 *
 * @param h Small decrement for the time to maturity.
 * @return The computed Theta value.
 */
double Option::theta() {
    double t1 = grid[S0_ / dS][1];
    double t2 = grid[S0_ / dS][0];

    return (t1 - t2) / (dT);
}

/**
 * @brief Computes the Vega of the option.
 *
 * Vega measures the sensitivity of the option price to changes in volatility \( \sigma \).
 * It is computed using finite differences as:
 * \[
 * \nu = \frac{\text{price}(\sigma + h) - \text{price}(\sigma)}{h}
 * \]
 *
 * @param h Small increment for the volatility.
 * @return The computed Vega value.
 */
double Option::vega(double h) {
    Option tmp(contract_type_, exercise_type_, T_, K_, T0_, time_mesh_, spot_mesh_, S0_, interest_rate_, volatility_ + h);

    return (tmp.price() - price()) / h;
}

/**
 * @brief Computes the Rho of the option.
 *
 * Rho measures the sensitivity of the option price to changes in the interest rate \( r \).
 * It is computed using finite differences as:
 * \[
 * \rho = \frac{\text{price}(r + h) - \text{price}(r)}{h}
 * \]
 *
 * @param h Small increment for the interest rate.
 * @return The computed Rho value.
 */

double Option::rho(double h) {
    std::vector<std::pair<double, double>> ir_tmp = interest_rate_;
    for (std::pair<double, double>& elem : ir_tmp) {
        elem.second += h;
    }
    Option tmp(contract_type_, exercise_type_, T_, K_, T0_, time_mesh_, spot_mesh_, S0_, ir_tmp, volatility_);

    return (tmp.price() - price()) / h;
}