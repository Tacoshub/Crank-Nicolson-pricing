/*
#include "Option.h"

int main() {

	try {
		int ct = 1; //call -> 1, put -> -1
		bool et = 0; //european -> 1, american -> 0
		double T = 1.0; //maturity
		double K = 150; //strike price
		double T0 = 0.0; //starting time
		double S0 = 200; //starting value of underlying
		std::vector<std::pair<double, double>> ir = { {0.0, 0.10}, {1.0, 0.10} }; //discrete interest rate curve

		InterestRate IR(ir);
		
		double sigma = 0.20; //volatility
		unsigned int N = 1000; //time mesh
		unsigned int M = 1000; //spot mesh

		Option call(1, et, T, K, T0, N, M, S0, ir, sigma);
		double call_price = call.price();
		std::cout << call_price << std::endl;
		Option put(-1, et, T, K, T0, N, M, S0, ir, sigma);
		double put_price = put.price();
		std::cout << put_price << std::endl;

		//std::cout << call_price - put_price - (S0 - K*std::exp(-IR.integral(0))) << std::endl;
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