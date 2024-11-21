/**
 * @file ImperialAmericanPut.h
 * @brief Imperial College pricer for American puts
 */

#pragma once
#include<iostream>
#include<cmath>
using namespace std;

double maxx(double a, double b)
{
	return ((a > b) ? a : b);
}
class CNicolsonPricer {
private:
	double Price;
public:
	CNicolsonPricer(double S_now, double K, double T, double
		Vol, double r, int I, double delta_t);
	double Value()
	{
		return Price;
	}
	~CNicolsonPricer() {}
};
CNicolsonPricer::CNicolsonPricer(double S_now, double K, double T, double
	Vol, double r, int I, double delta_t)
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
	//Boudary conitions (i.e. for i=0 & I):
	Vcurrent_j[0] = K;
	Vcurrent_j[I] = 0;
	//(2) Now let j>0:
	for (j = 1; j <= J; j++) {
		for (i = 1; i <= I - 1; i++)//SOR's initial guess
		{
			Vcurrent_j[i] = Vprevious_j[i];
		}
		for (i = 1; i <= I - 1; i++)
		{
			predictor[i] = a[i] * Vprevious_j[i - 1] + b[i] * Vprevious_j[i]
				+ c[i] * Vprevious_j[i + 1];
		}
		k = 0; // SOR iteration count initialised
		do {
			error = 0;
			for (i = 1; i <= I - 1; i++)
			{
				diff = (predictor[i] - A[i] * Vcurrent_j[i - 1] - C[i] *
					Vcurrent_j[i + 1]) / B[i] - Vcurrent_j[i];
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
