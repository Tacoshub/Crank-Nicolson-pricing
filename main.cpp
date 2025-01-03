#include "Option.h"

#include "Option.h"

int main() {

	try {
		int ct = 1; //call -> 1, put -> -1
		int et = 1; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 40.0; //strike price
		double T0 = 0.0; //starting time
		double S0 = 50.0; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, 0.0}, {1.0, 0.0212} }; //discrete interest rate curve

		double sigma = 0.1; //volatility
		unsigned int N = 500; //time mesh
		unsigned int M = 500; //spot mesh

		Option opt(ct, et, T, K, T0, N, M, S0, ir, sigma);
		std::cout << std::fixed << std::setprecision(5);
		std::cout << "Price: " << std::setw(5) << opt.price() << std::endl
			<< "Delta: " << std::setw(5) << opt.delta(S0) << std::endl
			<< "Gamma: " << std::setw(5) << opt.gamma() << std::endl
			<< "Theta: " << std::setw(5) << opt.theta() << std::endl
			<< "Vega:  " << std::setw(5) << opt.vega() << std::endl
			<< "Rho:   " << std::setw(5) << opt.rho() << std::endl;
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