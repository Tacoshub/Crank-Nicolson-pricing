/*!
 * \mainpage Advanced Option Pricing Framework
 *
 * \section intro_sec Introduction
 *
 * This project implements a comprehensive framework for pricing financial options using numerical methods.
 * It supports both European and American options, employing finite difference methods for solving the
 * partial differential equations governing option prices. The framework is designed to handle various
 * scenarios, including dynamic interest rates and early exercise features.
 *
 * \section features_sec Features
 *
 * The key features of this framework include:
 *
 * - **Support for Multiple Option Types:**
 *   - European and American options.
 *   - Call and Put contracts.
 *
 * - **Finite Difference Methods:**
 *   - Tridiagonal matrix computations for efficient numerical solutions.
 *   - Iterative methods with penalty adjustments for American options.
 *
 * - **Sensitivity Analysis (the Greeks):**
 *   - Delta, Gamma, Vega, Theta, and Rho are computed using finite differences.
 *
 * - **Dynamic Interest Rates:**
 *   - Flexible handling of interest rate curves.
 *
 * - **Grid-Based Pricing:**
 *   - A detailed computational grid for visualizing option price evolution over time and spot prices.
 *
 * \section usage_sec Usage
 *
 * To price an option, create an instance of the `Option` class with appropriate parameters:
 *
 * - **Contract and Exercise Types:** Use `1` for Call and `-1` for Put, `1` for European, and `0` for American.
 * - **Model Parameters:** Provide maturity \( T \), strike price \( K \), interest rate curve, and volatility.
 * - **Grid Resolution:** Set the number of time and spot price steps for the finite difference grid.
 *
 * Example:
 * ```cpp
 * #include "Option.h"
 *
 * int main() {
 *     // Parameters
 *     int contract_type = 1; // Call option
 *     int exercise_type = 0; // American option
 *     double T = 1.0;        // Maturity (1 year)
 *     double K = 100.0;      // Strike price
 *     double T0 = 0.0;       // Start time
 *     unsigned int time_mesh = 50; // Time steps
 *     unsigned int spot_mesh = 100; // Spot price steps
 *     double S0 = 100.0;     // Current spot price
 *     std::vector<std::pair<double, double>> interest_rate = {{0.0, 0.05}, {1.0, 0.05}};
 *     double volatility = 0.2; // Volatility (20%)
 *     double tol = 1e-6;       // Tolerance for iterative methods
 *     double w = 1.5;          // Relaxation parameter
 *
 *     // Create Option object
 *     Option opt(contract_type, exercise_type, T, K, T0, time_mesh, spot_mesh, S0, interest_rate, volatility, tol, w);
 *
 *     // Compute price
 *     double price = opt.price();
 *
 *     // Display results
 *     std::cout << "Option Price: " << price << std::endl;
 *     opt.display_grid(); // Show the pricing grid
 *     return 0;
 * }
 * ```
 *
 * \section impl_sec Implementation Details
 *
 * - **Finite Difference Grid:** A 2D grid of option values is created, with rows representing spot prices
 *   and columns representing time steps.
 * - **Boundary Conditions:** Applied to enforce realistic option values at extreme spot prices.
 * - **Tridiagonal Matrix Operations:** Efficiently solve the linear systems arising in finite difference methods.
 * - **Penalty Method:** Handles the early exercise feature of American options.
 *
 * \section greeks_sec Sensitivity Analysis
 *
 * The framework provides methods to compute the following option sensitivities (Greeks):
 * - **Delta:** Sensitivity to changes in the underlying asset price.
 * - **Gamma:** Sensitivity of Delta to changes in the underlying asset price.
 * - **Theta:** Sensitivity to the passage of time.
 * - **Vega:** Sensitivity to changes in volatility.
 * - **Rho:** Sensitivity to changes in the interest rate.
 *
 * \section future_sec Future Enhancements
 *
 * Suggestions for future development:
 * - Extend support for multi-asset options.
 * - Implement Monte Carlo simulations for validation.
 * - Add functionality for barrier and Asian options.
 * - Improve performance with GPU-based parallelism.
 *
 * \section author_sec Author
 *
 * Developed as part of an advanced computational finance project. This framework is intended for educational
 * purposes and as a foundation for further research in option pricing techniques.
 *
 * Authors:
 * Boccia Flavio
 * Costa Ludovico
 */