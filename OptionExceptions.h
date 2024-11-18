#pragma once

#include<stdexcept>
#include<string>

class OptionExceptions : std::exception {

public:

	virtual const char* what() const throw() {
		return "Generic error";
	}

};

class InvalidContractType : public OptionExceptions {

	std::string msg;

public:

	InvalidContractType(int N) {
		msg = "Invalid contract type, value must be 1 (Call) or -1 (Put), value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidExerciseType : public OptionExceptions {

	std::string msg;

public:

	InvalidExerciseType(int N) {
		msg = "Invalid exercise type, value must be 1 (European) or 0 (American), value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidMaturity : public OptionExceptions {

	std::string msg;

public:

	InvalidMaturity() {
		msg = "Invalid maturity";
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidStrike : public OptionExceptions {

	std::string msg;

public:

	InvalidStrike(int N) {
		msg = "Invalid strike, value must be positive, value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidTimeMesh : public OptionExceptions {

	std::string msg;

public:

	InvalidTimeMesh(int N) {
		msg = "Invalid value for time mesh, value must be positive, value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidSpotMesh : public OptionExceptions {

	std::string msg;

public:

	InvalidSpotMesh(int N) {
		msg = "Invalid value for time mesh, value must be positive, value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidSpot : public OptionExceptions {

	std::string msg;

public:

	InvalidSpot(int N) {
		msg = "Invalid value for spot price, value must be positive, value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidVolatility : public OptionExceptions {

	std::string msg;

public:

	InvalidVolatility(int N) {
		msg = "Invalid value for volatility, value must be positive, value received: ";
		msg += std::to_string(N);
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};

class InvalidTime : public OptionExceptions {

	std::string msg;

public:

	InvalidTime() {
		msg = "Value out of bounds";
	}
	virtual const char* what() const throw() {
		return msg.c_str();
	}

};
