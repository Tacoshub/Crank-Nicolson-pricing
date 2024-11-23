/**
 * @file ImperialAmericanPut.h
 * @brief Imperial College pricer for American puts, used for test cases
 */

#pragma once
#include <iostream>
#include <cmath>
using namespace std;

/**
 * @brief Returns the maximum of two values.
 *
 * This helper function is used to ensure that the value is non-negative, as is the case for options' intrinsic values.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 * @return The greater of the two values.
 */
double maxx(double a, double b)
{
    return ((a > b) ? a : b);
}

/**
 * @class CNicolsonPricer
 * @brief Class to price American put options using the Crank-Nicolson method with SOR iteration.
 *
 * This class uses a finite difference method with the Crank-Nicolson scheme, along with the SOR method for solving the linear system.
 */
class CNicolsonPricer {
private:
    double Price;

public:
    /**
     * @brief Constructor for the CNicolsonPricer class.
     *
     * Initializes the pricer with the necessary parameters and performs the pricing calculation.
     *
     * @param S_now Current spot price of the underlying asset.
     * @param K Strike price of the option.
     * @param T Time to maturity.
     * @param Vol Volatility of the underlying asset.
     * @param r Risk-free interest rate.
     * @param I Number of space steps for the finite difference grid.
     * @param delta_t Time step for the finite difference grid.
     */
    CNicolsonPricer(double S_now, double K, double T, double Vol, double r, int I, double delta_t);

    /**
     * @brief Returns the computed option price.
     *
     * @return The computed price of the option.
     */
    double Value()
    {
        return Price;
    }

    /**
     * @brief Destructor for the CNicolsonPricer class.
     *
     * Cleans up any dynamically allocated memory (if any).
     */
    ~CNicolsonPricer() {}
};

/**
 * @brief Constructor for the CNicolsonPricer class.
 *
 * This function initializes the pricer with the given parameters and calculates the price of the option using the Crank-Nicolson method.
 *
 * @param S_now Current spot price of the underlying asset.
 * @param K Strike price of the option.
 * @param T Time to maturity.
 * @param Vol Volatility of the underlying asset.
 * @param r Risk-free interest rate.
 * @param I Number of space steps for the finite difference grid.
 * @param delta_t Time step for the finite difference grid.
 */
CNicolsonPricer::CNicolsonPricer(double S_now, double K, double T, double Vol, double r, int I, double delta_t)
{
    int i, j, k;
    double error, diff;
    const double MAXk = 100;
    const double TOL = 0.00000001;
    const double DELTA_S = 3 * K / I;
    const int i_STAR = floor(S_now / DELTA_S);
    const double WEIGHT = (S_now - i_STAR * DELTA_S) / DELTA_S;
    const double J = floor(T / delta_t);
    delta_t = T / J;
    const double W = 1.5;

    double* predictor = new double[I + 1];
    double* Vprevious_j = new double[I + 1];
    double* Vcurrent_j = new double[I + 1];
    double* fix = new double[I + 1];
    double* a = new double[I + 1];
    double* b = new double[I + 1];
    double* c = new double[I + 1];
    double* A = new double[I + 1];
    double* B = new double[I + 1];
    double* C = new double[I + 1];

    for (i = 0; i <= I; i++)
    {
        Vprevious_j[i] = maxx(0, K - i * DELTA_S);
        fix[i] = Vprevious_j[i];
    }

    for (i = 1; i <= I - 1; i++)
    {
        a[i] = delta_t / 4 * (Vol * Vol * i * i - r * i);
        b[i] = (1 - delta_t / 2 * (r + Vol * Vol * i * i));
        c[i] = (delta_t / 4 * (Vol * Vol * i * i + r * i));
        A[i] = -a[i];
        B[i] = (1 + delta_t / 2 * (r + Vol * Vol * i * i));
        C[i] = -c[i];
    }

    Vcurrent_j[0] = K;
    Vcurrent_j[I] = 0;

    for (j = 1; j <= J; j++) {
        for (i = 1; i <= I - 1; i++)
        {
            Vcurrent_j[i] = Vprevious_j[i];
        }
        for (i = 1; i <= I - 1; i++)
        {
            predictor[i] = a[i] * Vprevious_j[i - 1] + b[i] * Vprevious_j[i] + c[i] * Vprevious_j[i + 1];
        }
        k = 0;
        do {
            error = 0;
            for (i = 1; i <= I - 1; i++)
            {
                diff = (predictor[i] - A[i] * Vcurrent_j[i - 1] - C[i] * Vcurrent_j[i + 1]) / B[i] - Vcurrent_j[i];
                error += diff * diff;
                Vcurrent_j[i] += W * diff;
            }
            k++;
        } while (error > TOL && k < MAXk);
        for (i = 1; i <= I - 1; i++)
            Vcurrent_j[i] = maxx(Vcurrent_j[i], fix[i]);
        for (i = 1; i <= I - 1; i++)
        {
            Vprevious_j[i] = Vcurrent_j[i];
        }
    }

    Price = (1 - WEIGHT) * Vprevious_j[i_STAR] + WEIGHT * Vprevious_j[i_STAR + 1];

    delete[] Vprevious_j;
    delete[] Vcurrent_j;
    delete[] predictor;
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] A;
    delete[] B;
    delete[] C;
}