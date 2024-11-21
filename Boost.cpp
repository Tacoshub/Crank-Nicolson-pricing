
#define BOOST_TEST_MODULE GridExplorationTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include "InterestRate.h" // Your InterestRate implementation
#include "Option.h"       // Your Option implementation
#include "ImperialAmericanPut.h"

// Black-Scholes formula for European options
double black_scholes_price(int ct, double S0, double K, double T, double r, double sigma) {
    double d1 = (std::log(S0 / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
    double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));

    if (ct == 1) { // Call
        return S0 * N_d1 - K * std::exp(-r * T) * N_d2;
    }
    else { // Put
        return K * std::exp(-r * T) * (1.0 - N_d2) - S0 * (1.0 - N_d1);
    }
}

// Global counter
unsigned int counter = 0;

BOOST_AUTO_TEST_SUITE(GridExploration)

// Common test parameters
std::vector<double> maturities = { 0.5, 1.0, 2.0 };
std::vector<double> strikes = { 80, 100, 120 };
std::vector<double> volatilities = { 0.1, 0.2, 0.3 };
//std::vector<double> interest_rates = { 0.0, -0.05, 0.05 };
std::vector<double> interest_rates = { 0.0};
double S0 = 100; // Spot price
unsigned int N = 1000; // Time steps
unsigned int M = 400; // Spot mesh



BOOST_AUTO_TEST_CASE(AmericanPutTest) {
    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    Option opt(-1, false, T, K, 0.0, N, M, S0, ir_curve, sigma); // American put
                    double computed_price = opt.price();
                    CNicolsonPricer CN(S0, K, T, sigma, r, 120, 0.005);
                    double bs_price = CN.Value();

                    // Print configuration
                    std::cout << "Configuration #" << counter + 1
                        << ": Type=American Put, T=" << T << ", K=" << K
                        << ", r=" << r << ", sigma=" << sigma
                        << ", Computed Price=" << computed_price
                        << ", Approx Black-Scholes Price=" << bs_price << "\n";

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // American put >= European put
                    counter++;
                }
            }
        }
    }
    std::cout << "Total combinations tested for American Put: " << counter << std::endl;
}




BOOST_AUTO_TEST_CASE(EuropeanCallTest) {
    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    Option opt(1, true, T, K, 0.0, N, M, S0, ir_curve, sigma); // European call
                    double computed_price = opt.price();
                    double bs_price = black_scholes_price(1, S0, K, T, r, sigma);

                    // Print configuration
                    std::cout << "Configuration #" << counter + 1
                        << ": Type=European Call, T=" << T << ", K=" << K
                        << ", r=" << r << ", sigma=" << sigma
                        << ", Computed Price=" << computed_price
                        << ", Black-Scholes Price=" << bs_price << "\n";

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // Compare with Black-Scholes
                    counter++;
                }
            }
        }
    }

    std::cout << "Total combinations tested for European Call: " << counter << std::endl;
}

BOOST_AUTO_TEST_CASE(EuropeanPutTest) {
    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    Option opt(-1, true, T, K, 0.0, N, M, S0, ir_curve, sigma); // European put
                    double computed_price = opt.price();
                    double bs_price = black_scholes_price(-1, S0, K, T, r, sigma);

                    // Print configuration
                    std::cout << "Configuration #" << counter + 1
                        << ": Type=European Put, T=" << T << ", K=" << K
                        << ", r=" << r << ", sigma=" << sigma
                        << ", Computed Price=" << computed_price
                        << ", Black-Scholes Price=" << bs_price << "\n";

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // Compare with Black-Scholes
                    counter++;
                }
            }
        }
    }

    std::cout << "Total combinations tested for European Put: " << counter << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()
