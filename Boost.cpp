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
unsigned int errors = 0;

BOOST_AUTO_TEST_SUITE(GridExploration)

// Common test parameters
std::vector<double> maturities = { 0.5, 1.0, 2.0 };
std::vector<double> strikes = { 80, 100, 120 };
std::vector<double> volatilities = { 0.1, 0.2, 0.3 };
//std::vector<double> interest_rates = { 0.0, -0.05, 0.05 };
std::vector<double> interest_rates = { 0.0};
double S0 = 100; // Spot price
unsigned int N = 100; // Time steps
unsigned int M = 100; // Spot mesh





BOOST_AUTO_TEST_CASE(EuropeanCallTest) {
    std::cout << "Testing European Calls..." << std::endl;

    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    Option opt(1, true, T, K, 0.0, N, M, S0, ir_curve, sigma); // European call
                    double computed_price = opt.price();
                    double bs_price = black_scholes_price(1, S0, K, T, r, sigma);   

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // Compare with Black-Scholes
                    if (std::abs(computed_price - bs_price) > 0.2 * bs_price)
                    {
                        errors += 1;
                    }
                    counter++;
                }
            }
        }
    }

    std::cout << "Total combinations tested for European Call: " << counter << std::endl;
    std::cout << "Total Errors for European Call: " << errors << std::endl;

}

BOOST_AUTO_TEST_CASE(EuropeanPutTest) {
    std::cout << "Testing European Puts..." << std::endl;

    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    Option opt(-1, true, T, K, 0.0, N, M, S0, ir_curve, sigma); // European put
                    double computed_price = opt.price();
                    double bs_price = black_scholes_price(-1, S0, K, T, r, sigma);
 

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // Compare with Black-Scholes
                    if (std::abs(computed_price - bs_price) > 0.2 * bs_price)
                    {
                        errors += 1;
                    }
                    counter++;
                }
            }
        }
    }

    std::cout << "Total combinations tested for European Put: " << counter << std::endl;
    std::cout << "Total errors  for European Put: " << errors << std::endl;

}

BOOST_AUTO_TEST_CASE(PutCallParityTest) {
    std::cout << "Testing Put Call Parity" << std::endl;

    for (double r : interest_rates) {
        std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
        InterestRate IR(ir_curve);

        for (double T : maturities) {
            for (double K : strikes) {
                for (double sigma : volatilities) {
                    // European call
                    Option call_opt(1, true, T, K, 0.0, N, M, S0, ir_curve, sigma);
                    double call_price = call_opt.price();

                    // European put
                    Option put_opt(-1, true, T, K, 0.0, N, M, S0, ir_curve, sigma);
                    double put_price = put_opt.price();

                    // Put-Call Parity Value
                    double parity_value = S0 - K * std::exp(-r * T);

                    // Check put-call parity
                    BOOST_TEST(std::abs((call_price - put_price) - parity_value) < call_price *0.2);
                    if (std::abs((call_price - put_price) - parity_value) > call_price * 0.2)
                    {
                        errors += 1;
                    }

                    counter++;
                }
            }
        }
    }

    std::cout << "Total combinations tested for Put-Call Parity: " << counter << std::endl;
    std::cout << "Total errors for Put-Call Parity: " << errors << std::endl;

}


BOOST_AUTO_TEST_CASE(AmericanPutTest) {
    std::cout << "Testing Ametican Put " << std::endl;

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

                    BOOST_TEST(std::abs(computed_price - bs_price) < 0.2 * bs_price); // American put >= European put
                    counter++;
                    if (std::abs(computed_price - bs_price) > 0.2 * bs_price)
                    {
                        errors += 1;
                    }
                }
            }
        }
    }
    std::cout << "Total combinations tested for American Put: " << counter << std::endl;
    std::cout << "Total Errors  for American Put: " << errors << std::endl;
}
BOOST_AUTO_TEST_CASE(EuropeanAndAmericanCallPriceEqualityTest) {
    for (double r : interest_rates) {
        if (r > 0) { // Only test for r > 0
            std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
            InterestRate IR(ir_curve);

            for (double T : maturities) {
                for (double K : strikes) {
                    for (double sigma : volatilities) {
                        // European call
                        Option european_call(1, true, T, K, 0.0, N, M, S0, ir_curve, sigma);
                        double european_price = european_call.price();

                        // American call
                        Option american_call(1, false, T, K, 0.0, N, M, S0, ir_curve, sigma);
                        double american_price = american_call.price();

                        // Print configuration
                        

                        // Check equality
                        BOOST_TEST(std::abs(european_price - american_price) >0.01* european_price); // Allow small numerical error

                        if (std::abs((european_price - american_price) >0.01* european_price))
                        {
                            errors += 1;
                        }

                        counter++;
                    }
                }
            }
        }
    }

    std::cout << "Total combinations tested for European and American Call Price Equality: " << counter << std::endl;
    std::cout << "Total Errors for European and American Call Price Equality: " << counter << std::endl;

}

BOOST_AUTO_TEST_CASE(EuropeanAndAmericanPutPriceEqualityTest) {
    for (double r : interest_rates) {
        if (r < 0) { // Only test for r < 0
            std::vector<std::pair<double, double>> ir_curve = { {0.0, r}, {10.0, r}, {20.0, r} };
            InterestRate IR(ir_curve);

            for (double T : maturities) {
                for (double K : strikes) {
                    for (double sigma : volatilities) {
                        // European put
                        Option european_put(-1, true, T, K, 0.0, N, M, S0, ir_curve, sigma);
                        double european_price = european_put.price();

                        // American put
                        Option american_put(-1, false, T, K, 0.0, N, M, S0, ir_curve, sigma);
                        double american_price = american_put.price();

                        // Print configuration
                       

                        // Check equality
                        BOOST_TEST(std::abs(european_price - american_price) < 0.01); // Allow small numerical error

                        if (std::abs((european_price - american_price) > 0.01 * european_price))
                        {
                            errors += 1;
                        }



                        counter++;
                    }
                }
            }
        }
    }

    std::cout << "Total combinations tested for European and American Put Price Equality: " << counter << std::endl;
    std::cout << "Total Errrors for European and American Put Price Equality: " << counter << std::endl;

}


BOOST_AUTO_TEST_SUITE_END()

