/**
 * @file mainpage.h
 * @brief Main page for the Advanced Option Pricing Framework.
 */

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
  *     try {
  *         int ct = 1; // Call -> 1, Put -> -1
  *         int et = 1; // European -> 1, American -> 0
  *         double T = 1.0; // Maturity
  *         double K = 40.0; // Strike price
  *         double T0 = 0.0; // Starting time
  *         double S0 = 50.0; // Starting value of underlying
  *         std::vector<std::pair<double, double>> ir = { {0.0, 0.0}, {1.0, 0.0212} }; // Discrete interest rate curve
  *
  *         double sigma = 0.1; // Volatility
  *         unsigned int N = 500; // Time mesh
  *         unsigned int M = 500; // Spot mesh
  *
  *         Option opt(ct, et, T, K, T0, N, M, S0, ir, sigma);
  *         std::cout << std::fixed << std::setprecision(5);
  *         std::cout << "Price: " << std::setw(5) << opt.price()         << std::endl
  *                   << "Delta: " << std::setw(5) << opt.delta(S0)       << std::endl
  *                   << "Gamma: " << std::setw(5) << opt.gamma()         << std::endl
  *                   << "Theta: " << std::setw(5) << opt.theta()         << std::endl
  *                   << "Vega:  " << std::setw(5) << opt.vega(sigma * h) << std::endl
  *                   << "Rho:   " << std::setw(5) << opt.rho(h)          << std::endl;
  *     } catch (const OptionExceptions& e) {
  *         std::cout << "Exception -> " << e.what();
  *     } catch (const std::exception& e) {
  *         std::cout << "General exception: " << e.what();
  *     } catch (...) {
  *         std::cout << "Unknown exception";
  *     }
  *
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
  * Boccia Flavio, Costa Ludovico
  * 
  * Contacts:
  * flaviosalvatore.boccia@gmail.com
  * ludocosta2002@gmail.com
  */