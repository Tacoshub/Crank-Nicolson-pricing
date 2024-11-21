#include "Option.h"
/*
int main() {

	try {
		int ct = 1; //call -> 1, put -> -1
		bool et = 1; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 180; //strike price
		double T0 = 0.0; //starting time
		double S0 = 200; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, 0.10}, {1.0, 0.10} }; //discrete interest rate curve

		InterestRate IR(ir);
		
		double sigma = 0.20; //volatility
		unsigned int N = 1000; //time mesh
		unsigned int M = 1000; //spot mesh
		double h = 0.01;

		Option call(ct, et, T, K, T0, N, M, S0, ir, sigma);
		std::cout << "Price: " << call.price()         << std::endl
			      << "Delta: " << call.delta(S0 * h)   << std::endl
			      << "Gamma: " << call.gamma(S0 * h)   << std::endl
			      << "Theta: " << call.theta(T * h)    << std::endl
			      << "Vega: "  << call.vega(sigma * h) << std::endl
			      << "Rho: "   << call.rho(h);
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