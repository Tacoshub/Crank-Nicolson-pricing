# Advanced Option Pricing Framework

## Introduction

This project implements a comprehensive framework for pricing financial options using numerical methods. It supports both European and American options, employing finite difference methods for solving the partial differential equations governing option prices. The framework is designed to handle various scenarios, including dynamic interest rates and early exercise features.

## Derivation

The numerical framework for option pricing begins with the **Black-Scholes Partial Differential Equation (PDE)**:

$$
\frac{\partial V}{\partial t} + \frac{1}{2} \sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} + r S \frac{\partial V}{\partial S} - r V = 0
$$

To solve this PDE numerically, we employ the **Crank-Nicolson Method**, which is a finite difference technique. The Crank-Nicolson scheme is derived by taking the average of the Forward Euler and Backward Euler discretizations of the Black-Scholes PDE. This results in a method that is second-order accurate in both time and space, while also being unconditionally stable for parabolic PDEs.

### Derivation steps

- **Discretization Using Finite Differences:**
The time and spatial derivatives in the Black-Scholes PDE are discretized as follows

$$
\frac{\partial V}{\partial t}(S_i, t_n) \approx \frac{V(S_i, t_{n+1}) - V(S_i, t_{n})}{\Delta t}
$$

$$
\frac{\partial V}{\partial S}(S_i, t_n) \approx \frac{V(S_{i+1}, t_n) - V(S_{i-1}, t_n)}{2\Delta S}
$$

$$
\frac{\partial^2 V}{\partial S^2}(S_i, t_n) \approx \frac{V(S_{i+1}, t_n) - 2V(S_i, t_n)  + V(S_{i-1}, t_n)}{\Delta S^2}
$$

- **Crank-Nicolson Averaging:**
By taking the average of the Forward Euler and Backward Euler discretizations, we obtain the Crank-Nicolson scheme. Grouping the terms together results in the following system of equations

$$
-a_{j,n}V(S_{i-1}, t_n) + (1-b_{j,n})V(S_i, t_n) - c_{j,n}V(S_{i+1}, t_n) = a_{j,n+1}V(S_{i-1}, t_{n+1}) + (1+b_{j,n+1})V(S_i, t_{n+1}) + c_{j,n}V(S_{i+1}, t_{n+1})
$$

where

$$
a_{j,n} = \frac{{\Delta t}}{4}j(\sigma^2j - r(t_n))
$$

$$
b_{j,n} = -\frac{{\Delta t}}{2}j(\sigma^2j^2 - r(t_n))
$$

$$
c_{j,n} = \frac{{\Delta t}}{4}j(\sigma^2j + r(t_n))
$$

- **Matrix Formulation:**
The method can be expressed in matrix form as:

$$
CV_{n} = DV_{n+1} + K_{n} + K_{n+1}, \space \space \space n = N, ..., 1
$$

where $V_{n}$ is the price vector with such that $(V_{n})_i = V(S_i, t_n)$. The matrices C and D are tridiagonal, with coefficients derived in the previous section while 
$K_n$ is a vector that incorporates the boundary conditions.

- **Key Feature:**
This formulation is particularly significant as it accomodates **time-varying interest rates**, which are often not adressed in the literature that assumes constant interest rate. By including this flexibility, the method provides a robust framework for pricing options in more realistic financial scenarios.

## Features

The key features of this framework include:

- **Support for Multiple Option Types:**
  - European and American options.
  - Call and Put contracts.

- **Finite Difference Methods:**
  - Tridiagonal matrix computations for efficient numerical solutions.
  - Iterative methods with penalty adjustments for American options.

- **Sensitivity Analysis (the Greeks):**
  - Delta, Gamma, Vega, Theta, and Rho are computed using finite differences.

- **Dynamic Interest Rates:**
  - Flexible handling of interest rate curves.

- **Grid-Based Pricing:**
  - A detailed computational grid for visualizing option price evolution over time and spot prices.

## Usage

To price an option, create an instance of the `Option` class with appropriate parameters:

- **Contract and Exercise Types:** Use `1` for Call and `-1` for Put, `1` for European, and `0` for American.
- **Model Parameters:** Provide maturity \( T \), strike price \( K \), interest rate curve, and volatility.
- **Grid Resolution:** Set the number of time and spot price steps for the finite difference grid.

### Example

```cpp
#include "Option.h"

#include "Option.h"

int main() {

	try {
		int ct = 1; //call -> 1, put -> -1
		int et = 1; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 1.0; //strike price
		double T0 = 0.0; //starting time
		double S0 = 1; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, 0.0}, {1.0, 0.0212} }; //discrete interest rate curve
		
		double sigma = 0.1; //volatility
		unsigned int N = 400; //time mesh
		unsigned int M = 400; //spot mesh
		double h = 0.01; //discretization parameter

		Option opt(ct, et, T, K, T0, N, M, S0, ir, sigma);
		std::cout << std::fixed << std::setprecision(5);
		std::cout << "Price: " << std::setw(5) << opt.price()         << std::endl
			      << "Delta: " << std::setw(5) << opt.delta(S0)       << std::endl
			      << "Gamma: " << std::setw(5) << opt.gamma()         << std::endl
			      << "Theta: " << std::setw(5) << opt.theta()         << std::endl
			      << "Vega:  " << std::setw(5) << opt.vega(sigma * h) << std::endl
			      << "Rho:   " << std::setw(5) << opt.rho(h)          << std::endl;
	}
	catch (const OptionExceptions& e) {
		std::cout << "Exception -> " << e.what();
	}
	catch (const std::exception& e) {
		std::cout << "General exception: " << e.what();
	}
	catch (...) {
		std::cout << "Unknown exception";
	}

    return 0;
}

```

## Implementation Details

- **Finite Difference Grid:** A 2D grid of option values is created, with rows representing spot prices and columns representing time steps.

- **Boundary Conditions:** Applied to enforce realistic option values at extreme spot prices.

- **Tridiagonal Matrix Operations:** Efficiently solve the linear systems arising in finite difference methods.

- **Penalty Method:** Handles the early exercise feature of American options.

## Sensitivity Analysis

The framework provides methods to compute the following option sensitivities (Greeks):

- **Delta:** Sensitivity to changes in the underlying asset price.

- **Gamma:** Sensitivity of Delta to changes in the underlying asset price.

- **Theta:** Sensitivity to the passage of time.

- **Vega:** Sensitivity to changes in volatility.

- **Rho:** Sensitivity to changes in the interest rate.

## Future Enhancements

Suggestions for future development:

- Extend support for multi-asset options.

- Implement Monte Carlo simulations for validation.

- Add functionality for barrier and Asian options.

- Improve performance with GPU-based parallelism.

## Author

Developed as part of an advanced computational finance project. This framework is intended for educational purposes and as a foundation for further research in option pricing techniques.

Authors: Boccia Flavio, Costa Ludovico

Contacts: flaviosalvatore.boccia@gmail.com, ludocosta2002@gmail.com

## Precise Derivation Of The Scheme

The following provides a precise derivation of the Crank-Nicolson scheme:
[Crank Nicolson Scheme.pdf](https://github.com/user-attachments/files/18387636/Crank.Nicolson.Scheme.pdf)
