/**
 * @file Option.cpp
 * @brief contains various methods to deal with the Option object
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
 * The norm is calculated as \( \sqrt{\sum_i v_i^2} \).
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
 * Initializes the option with specified contract and exercise type, maturity,
 * strike price, and other model parameters.
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

	dT = 0;
	dS = 0;

	curve = InterestRate(interest_rate_);

	if (contract_type == 1) { F0 = 0, FM = 5 * S0; }
	else { F0 = K, FM = 0; }
}

/**
 * @brief Creates the grid for option pricing.
 *
 * Initializes a 2D grid for pricing computations based on `spot_mesh_` and `time_mesh_`.
 */
void Option::create_grid() {
	std::vector<std::vector<double>> matrix(spot_mesh_ + 1, std::vector<double>(time_mesh_, 0.0));
	grid = matrix;
}

/**
 * @brief Computes coefficients \( a_j \) for the tridiagonal matrix in the finite difference method.
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
 * where:
 * - \( a \): Subdiagonal coefficients (negative of input \( a \)).
 * - \( b \): Diagonal coefficients (1 minus input \( b \)).
 * - \( c \): Superdiagonal coefficients (negative of input \( c \)).
 *
 * @param a Vector of subdiagonal coefficients.
 * @param b Vector of diagonal coefficients.
 * @param c Vector of superdiagonal coefficients.
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
 * where:
 * - \( a \): Subdiagonal coefficients.
 * - \( b \): Diagonal coefficients (1 plus input \( b \)).
 * - \( c \): Superdiagonal coefficients.
 *
 * @param a Vector of subdiagonal coefficients.
 * @param b Vector of diagonal coefficients.
 * @param c Vector of superdiagonal coefficients.
 * @return Tridiag object representing the matrix \( D \).
 */
Tridiag Option::compute_D(std::vector<double> a, std::vector<double> b, std::vector<double> c) {
	return Tridiag(a, 1.0 + b, c);
}

/**
 * @brief Computes the boundary terms \( K_1 \) and \( K_2 \) used for pricing adjustments at the boundaries.
 *
 * The boundary terms are computed as:
 * \[
 * K_1 = a_1^{\text{prec}} F_0 e^{-\int r \, dt} + a_1^{\text{curr}} F_0 e^{-\int r \, dt}
 * \]
 * \[
 * K_2 = c_m^{\text{prec}} \left(F_M - K e^{-\int r \, dt}\right) + c_m^{\text{curr}} \left(F_M - K e^{-\int r \, dt}\right)
 * \]
 * where:
 * - \( a_1 \): Coefficient for the first boundary.
 * - \( c_m \): Coefficient for the last boundary.
 * - \( F_0 \), \( F_M \): Boundary values of the grid.
 * - \( r \): Interest rate.
 *
 * @param i Time step index.
 * @return A pair of boundary terms \( (K_1, K_2) \).
 */
std::pair<double, double> Option::compute_K(unsigned int i) {
	// First boundary term (K1)
	double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);
	double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);
	double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) + a1_curr * F0 * std::exp(-curve.integral(dT * i));

	// Last boundary term (K2)
	double cm_prec = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * (i - 1)) * (spot_mesh_ - 1));
	double cm_curr = (dT / 4) * (volatility_ * volatility_ * (spot_mesh_ - 1) * (spot_mesh_ - 1) - curve(dT * i) * (spot_mesh_ - 1));
	double K2 = cm_prec * (FM - K_ * std::exp(-curve.integral(dT * (i - 1)))) * (contract_type_ == 1) +
		cm_curr * (FM - K_ * std::exp(-curve.integral(dT * i))) * (contract_type_ == 1);

	return std::make_pair(K1, K2);
}

/**
 * @brief Computes the boundary term \( K_1 \) used for American option pricing adjustments at the lower boundary.
 *
 * The boundary term \( K_1 \) for American options is computed as:
 * \[
 * K_1 = a_1^{\text{prec}} F_0 e^{-\int r \, dt} + a_1^{\text{curr}} F_0 e^{-\int r \, dt}
 * \]
 * where:
 * - \( a_1 \): Coefficients based on the volatility and interest rate at previous and current time steps.
 * - \( F_0 \): Boundary value at the lower boundary.
 * - \( r \): Interest rate.
 *
 * This term is specific to American options and applies only to the lower boundary of the pricing grid.
 *
 * @param i Time step index.
 * @return The computed boundary term \( K_1 \) for the American option.
 */
double Option::compute_K_american(unsigned int i) {
	// Coefficient for the lower boundary at the previous time step
	double a1_prec = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * (i - 1)) * 1);

	// Coefficient for the lower boundary at the current time step
	double a1_curr = (dT / 4) * (volatility_ * volatility_ * 1 * 1 - curve(dT * i) * 1);

	// Boundary term computation
	double K1 = a1_prec * F0 * std::exp(-curve.integral(dT * (i - 1))) +
		a1_curr * F0 * std::exp(-curve.integral(dT * i));

	return K1;
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
double Option::delta(double h) {
	double price1 = price();
	S0_ += h;
	double price2 = price();
	S0_ -= h;

	return (price2 - price1) / h;
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
double Option::gamma(double h) {
	double delta1 = delta(h);
	S0_ += h;
	double delta2 = delta(h);
	S0_ -= h;

	return (delta2 - delta1) / h;
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
double Option::theta(double h) {
	double price1 = price();
	T_ -= h;
	double price2 = price();
	T_ += h;
	return (price2 - price1) / h;
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
	double price1 = price();
	volatility_ += h;
	double price2 = price();
	volatility_ -= h;

	return (price2 - price1) / h;
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
	double price1 = price();
	curve += h;
	double price2 = price();
	curve -= h;

	return (price2 - price1) / h;
}






/**
 * @brief Computes the price of a European option using the finite difference method.
 *
 * This method solves the pricing problem by iterating backward in time over the grid:
 * - At each time step, it constructs tridiagonal matrices \( C \) and \( D \) using the coefficients \( a_j, b_j, c_j \).
 * - It computes the right-hand side of the equation \( \text{RHS} = D \cdot F + K \), where \( F \) is the solution from the previous step.
 * - It solves the linear system \( C \cdot F = \text{RHS} \) to update the values of \( F \).
 * - Boundary conditions are applied at each time step for the grid values.
 */
void Option::european_price() {
	Tridiag C, D;
	std::pair<double, double> K;
	std::vector<double> a, b, c, RHS;
	size_t zz;

	// Backward iteration over time steps
	for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
		// Compute coefficients for tridiagonal matrices
		a = compute_aj(jj);
		b = compute_bj(jj);
		c = compute_cj(jj);

		// Construct matrices C and D
		C = compute_C(a, b, c);
		D = compute_D(a, b, c);

		// Compute boundary terms
		K = compute_K(jj);

		// Compute RHS of the system
		RHS = D * F + K;

		// Solve the system C * F = RHS
		F = C.solve(RHS);

		// Apply boundary conditions to the grid
		grid[0][jj - 1] = F0 * std::exp(-curve.integral(dT * (jj - 1)));
		for (zz = 1; zz < spot_mesh_; zz++) {
			grid[zz][jj - 1] = F[zz - 1];
		}
		grid[zz][jj - 1] = (FM - K_ * std::exp(-curve.integral(dT * (jj - 1)))) * (contract_type_ == 1);
	}
}

/**
 * @brief Computes the price of an American option using the finite difference method with the penalty approach.
 *
 * This method iterates backward in time over the grid:
 * - At each time step, it constructs the tridiagonal matrix \( D \) and computes the right-hand side \( \text{RHS} \).
 * - It uses an iterative method to enforce the early exercise condition by introducing a penalty term.
 * - The grid values are updated at each step with the maximum of the intrinsic value and the continuation value.
 * - Boundary conditions are applied for each time step.
 */
void Option::american_price() {
	std::vector<double> a, b, c;
	Tridiag D;
	double K, Sk;
	std::vector<double> RHS;
	size_t ii, zz;

	// Backward iteration over time steps
	for (size_t jj = time_mesh_ - 1; jj > 0; jj--) {
		// Compute coefficients for tridiagonal matrix
		a = compute_aj(jj);
		b = compute_bj(jj);
		c = compute_cj(jj);

		// Construct matrix D
		D = compute_D(a, b, c);

		// Compute boundary term for American options
		K = compute_K_american(jj);

		// Compute RHS of the system
		RHS = D * F + K;

		// Iterative method to enforce early exercise condition
		double error = 1e6; // Initial large error
		std::vector<double> F_tmp(spot_mesh_ - 1); // Temporary solution vector

		while (error > tol_) {
			Sk = dS; // Reset underlying asset price
			// First node
			F_tmp[0] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
				F[0] + (w_ / (1 - b[0])) * (RHS[0] - (1 - b[0]) * F[0] + c[0] * F[1]));

			// Intermediate nodes
			ii = 1;
			for (; ii < spot_mesh_ - 2; ii++) {
				Sk += dS;
				F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
					F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] -
						(1 - b[ii]) * F[ii] + c[ii] * F[ii + 1]));
			}

			// Last node
			Sk += dS;
			F_tmp[ii] = std::max(std::max(contract_type_ * (Sk - K_), 0.0),
				F[ii] + (w_ / (1 - b[ii])) * (RHS[ii] + a[ii - 1] * F_tmp[ii - 1] - (1 - b[ii]) * F[ii]));

			// Update error and solution
			error = norm(F - F_tmp);
			F = F_tmp;
		}

		// Apply boundary conditions to the grid
		grid[0][jj - 1] = F0 * std::exp(-curve.integral(dT * (jj - 1)));
		for (zz = 1; zz < spot_mesh_; zz++) {
			grid[zz][jj - 1] = F[zz - 1];
		}
		grid[zz][jj - 1] = (FM - K_ * std::exp(-curve.integral(dT * (jj - 1)))) * (contract_type_ == 1);
	}
}

/**
 * @brief Solves the option pricing problem by setting up the initial grid and performing the computation.
 *
 * This method initializes the grid values at maturity based on the payoff function:
 * \[
 * F(S, T) = \max(\text{contract\_type} \cdot (S - K), 0)
 * \]
 * Then it uses the appropriate method (`european_price` or `american_price`) to compute the option prices
 * at earlier time steps based on the exercise type.
 */
void Option::solve() {
	double Sk = 0; // Underlying asset price
	size_t ii = 0;

	// Resize the vector F to hold spot_mesh_ - 1 elements
	F.resize(spot_mesh_ - 1);

	// Initialize the grid at maturity
	for (; ii <= spot_mesh_; ii++) {
		grid[ii][time_mesh_ - 1] = std::max(contract_type_ * (Sk - K_), 0.0);
		if (ii != 0 && ii != spot_mesh_) {
			F[ii - 1] = grid[ii][time_mesh_ - 1];
		}
		Sk += dS;
	}

	// Compute option prices for earlier time steps
	if (exercise_type_) {
		european_price(); // Solve for European options
	}
	else {
		american_price(); // Solve for American options
	}
}

/**
 * @brief Computes the price of the option using the finite difference method.
 *
 * This method initializes the time step size \( \Delta T \) and spot price step size \( \Delta S \),
 * creates the computational grid, and solves the problem using the `solve` method.
 *
 * @return The computed option price at the initial time and spot price \( S_0 \).
 */
double Option::price() {
	dT = (T_ - T0_) / (time_mesh_); // Time step size
	dS = (5 * S0_) / (spot_mesh_); // Spot price step size
	create_grid(); // Initialize the grid
	solve(); // Solve the pricing problem
	return grid[S0_ / dS][0]; // Return the option price at the initial node
}

/**
 * @brief Displays the pricing grid in a formatted table.
 *
 * Outputs the grid values to the console, showing the option price at each time and spot price step.
 * Each row corresponds to a spot price step, and each column corresponds to a time step.
 */
void Option::display_grid() {
	std::cout << std::fixed << std::setprecision(3); // Set fixed-point notation and precision
	for (size_t ii = 0; ii <= spot_mesh_; ii++) {
		for (size_t jj = 0; jj < time_mesh_; jj++) {
			std::cout << std::setw(8) << grid[ii][jj] << " "; // Format and print each grid value
		}
		std::cout << '\n'; // Newline after each row
	}
}





/**
 * @brief Generates a plot of the option price as a function of the spot price.
 *
 * This function computes a grid of option prices and extracts the values
 * for a fixed time slice to create a plot of option prices against spot prices.
 *
 * @return A vector of pairs, where each pair contains a spot price and the corresponding option price.
 */
std::vector<std::pair<double, double>> Option::price_plot() {
	dT = (T_ - T0_) / (time_mesh_); // Time step size
	dS = (5 * S0_) / (spot_mesh_);  // Spot price step size
	create_grid(); // Create the grid for the numerical method
	solve();       // Solve the option pricing problem

	std::vector<std::pair<double, double>> graph(spot_mesh_ - 1); // Store the results
	double S = dS; // Initial spot price
	std::pair<double, double> tmp; // Temporary variable for storing (spot price, option price)

	for (size_t ii = 1; ii < spot_mesh_; ii++) {
		tmp = std::make_pair(S, grid[ii][0]); // Extract the option price at the first time slice
		graph[ii - 1] = tmp; // Save the result
		S += dS; // Increment the spot price
	}

	return graph; // Return the price plot
}

/**
 * @brief Generates a plot of the option's delta as a function of the spot price.
 *
 * This function computes the delta (rate of change of the option price with respect
 * to the spot price) at various spot prices and stores the results in a vector.
 *
 * @param h A small perturbation used for numerical differentiation.
 * @return A vector of pairs, where each pair contains a spot price and the corresponding delta.
 */
std::vector<std::pair<double, double>> Option::delta_plot(double h) {
	std::vector<std::pair<double, double>> graph; // Store the results
	double S_min = S0_, S_max = 4 * S0_; // Range of spot prices
	double S = S_min; // Starting spot price
	std::pair<double, double> tmp; // Temporary variable for storing (spot price, delta)

	while (S <= S_max) {
		S0_ = S; // Set the current spot price
		tmp = std::make_pair(S, delta(S * h)); // Compute delta and store the result
		graph.push_back(tmp); // Save the result
		S += 0.1 * S_min; // Increment the spot price
	}
	S0_ = S_min; // Reset the spot price to its original value

	return graph; // Return the delta plot
}

/**
 * @brief Computes the exercise boundary for the option.
 *
 * This function determines the boundary separating the exercise and hold regions
 * for the option over time. The boundary is identified by finding the spot price
 * at each time step where the difference between the option value and the payoff is minimal.
 *
 * @return A vector of pairs, where each pair contains a time step and the corresponding exercise boundary spot price.
 */
std::vector<std::pair<double, double>> Option::exercise_boundary() {
	dT = (T_ - T0_) / (time_mesh_); // Time step size
	dS = (5 * S0_) / (spot_mesh_);  // Spot price step size
	create_grid(); // Create the grid for the numerical method
	solve();       // Solve the option pricing problem

	std::vector<std::pair<double, double>> graph; // Store the results
	double min, dist, S = 0; // Variables for tracking the minimum difference and corresponding spot price

	// Iterate over the time steps (excluding a small fraction of the end to avoid artifacts)
	for (size_t ii = 0; ii < time_mesh_ - 1; ii++) {
		min = 10000000; // Initialize the minimum distance to a large value

		// Iterate over the spot prices
		for (size_t jj = 1; jj < spot_mesh_; jj++) {
			dist = abs(grid[jj][ii] - contract_type_ * (dS * jj - K_)); // Compute the distance to the payoff
			if (dist < min) { // Update the minimum distance and corresponding spot price
				min = dist;
				S = dS * jj;
			}
		}
		graph.push_back(std::make_pair(ii * dT, S)); // Save the result
	}

	return graph; // Return the exercise boundary
}