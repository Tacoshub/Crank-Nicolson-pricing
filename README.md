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

```cpp
#include "Option.h"

int main() {

	try {
		int ct = -1; //call -> 1, put -> -1
		bool et = 1; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 180; //strike price
		double T0 = 0.0; //starting time
		double S0 = 190; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, 0.10}, {1.0, 0.10} }; //discrete interest rate curve
		
		double sigma = 0.20; //volatility
		unsigned int N = 1000; //time mesh
		unsigned int M = 1000; //spot mesh
		double h = 0.01;

		Option call(ct, et, T, K, T0, N, M, S0, ir, sigma);
		std::cout << std::fixed << std::setprecision(5);
		std::cout << "Price: " << std::setw(5) << call.price()         << std::endl
			      << "Delta: " << std::setw(5) << call.delta(S0 * h)   << std::endl
			      << "Gamma: " << std::setw(5) << call.gamma(S0 * h)   << std::endl
			      << "Theta: " << std::setw(5) << call.theta(T * h)    << std::endl
			      << "Vega:  " << std::setw(5) << call.vega(sigma * h) << std::endl
			      << "Rho:   " << std::setw(5) << call.rho(h)          << std::endl;
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

## Authors:
- Boccia Flavio
- Costa Ludovico
