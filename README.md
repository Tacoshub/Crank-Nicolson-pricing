# Crank-Nicolson Option Pricer

A comprehensive numerical framework for pricing European and American options using the Crank-Nicolson finite difference method. This pricer is designed for educational purposes and supports advanced features like dynamic interest rates, iterative methods, and sensitivity analysis (the Greeks).

## Features

- **Option Types:**
  - European options (no early exercise).
  - American options (with early exercise feature).

- **Finite Difference Method:**
  - Crank-Nicolson scheme for numerical stability and accuracy.
  - Successive Over-Relaxation (SOR) for handling non-linearity in American options.

- **Greeks Computation:**
  - Delta, Gamma, Theta, Vega, Rho.

- **Dynamic Interest Rates:**
  - Supports time-dependent interest rate curves.

- **Grid-Based Visualization:**
  - Computational grid to observe option price evolution over time and asset prices.

## Usage

- **Example:**

#include "Option.h"

int main() {

    // Variables
    int contract_type = 1; // Call option
    int exercise_type = 0; // American option
    double T = 1.0;        // Maturity (1 year)
    double K = 100.0;      // Strike price
    double T0 = 0.0;       // Start time
    unsigned int time_mesh = 50; // Time steps
    unsigned int spot_mesh = 100; // Spot price steps
    double S0 = 100.0;     // Current spot price
    std::vector<std::pair<double, double>> interest_rate = {{0.0, 0.05}, {1.0, 0.05}};
    double volatility = 0.2; // Volatility (20%)
    double tol = 1e-6;       // Tolerance for iterative methods
    double w = 1.5;          // Relaxation parameter

    // Create Option object
    Option opt(contract_type, exercise_type, T, K, T0, time_mesh, spot_mesh, S0, interest_rate, volatility, tol, w);

    // Compute price
    double price = opt.price();

    // Display results
    std::cout << "Option Price: " << price << std::endl;
    opt.display_grid(); // Show the pricing grid
    return 0;
}

## Authors:
- Boccia Flavio
- Costa Ludovico
