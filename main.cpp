#include "Option.h"
/*
int main() {

	try {
		int ct = 1; //call -> 1, put -> -1
		bool et = 0; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 150; //strike price
		double T0 = 0.0; //starting time
		double S0 = 100; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, -0.05}, {10.0, -0.05}, {20.0, -0.05} }; //discrete interest rate curve

		InterestRate IR(ir); //object to represent the interest rate curve
		
		double sigma = 0.20; //volatility
		unsigned int N = 1000; //time mesh
		unsigned int M = 1000; //spot mesh

		Option prova(ct, et, T, K, T0, N, M, S0, ir, sigma);
		//prova.display_grid();
		prova.price();
	}
	catch (const OptionExceptions& e) {
		std::cout << "Exception -> " << e.what();
	}
	catch (const std::exception& e) {
		std::cout << "General exception";
	}
	catch (...) {
		std::cout << "Unknown exception";
	}

    return 0;
}*/