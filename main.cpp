#include "Option.h"
#include "Tridiag.h"

#include<iostream>
int main() {

	int ct = 1;
	int et = 1;
	double T = 1.0;
	double K = 1.0;
	double T0 = 0.0;
	double S0 = 1.0;
	std::vector<std::pair<double, double>> ir = { {0.0, 0}, {10.0, 1}, {20.0, 0} };

	InterestRate IR(ir);
	std::cout << IR.integral(5,20);

	/*
	double sigma = 0.25;
	unsigned int N = 11;
	unsigned int M = 500;

	Option prova(ct, et, T, K, T0, N, M, S0, ir, sigma);
	prova.display_grid();
	std::cout << std::endl;
	prova.price();

	
    return 0;
	*/
}