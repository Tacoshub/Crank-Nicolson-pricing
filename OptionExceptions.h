/**
 * @file OptionExceptions.h
 * @brief Contains custom exception classes for handling errors in option processing.
 */
#pragma once

#include <stdexcept>
#include <string>

 /**
  * @brief Base class for handling errors related to option processing.
  */
class OptionExceptions : public std::exception {
public:
    /**
     * @brief Default error message for generic errors.
     * @return A constant C-string representing the error message.
     */
    virtual const char* what() const throw() {
        return "Generic error";
    }
};

/**
 * @brief Exception thrown when an invalid contract type is encountered.
 *
 * Contract types must be 1 (Call) or -1 (Put).
 */
class InvalidContractType : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid contract type.
     * @param N The invalid contract type received.
     */
    InvalidContractType(int N) {
        msg = "Invalid contract type, value must be 1 (Call) or -1 (Put), value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid exercise type is encountered.
 *
 * Exercise types must be 1 (European) or 0 (American).
 */
class InvalidExerciseType : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid exercise type.
     * @param N The invalid exercise type received.
     */
    InvalidExerciseType(int N) {
        msg = "Invalid exercise type, value must be 1 (European) or 0 (American), value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid strike price is encountered.
 *
 * Strike price must be positive.
 */
class InvalidStrike : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid strike price.
     * @param N The invalid strike price received.
     */
    InvalidStrike(double N) {
        msg = "Invalid strike, value must be positive, value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid time mesh value is encountered.
 *
 * Time mesh must be positive.
 */
class InvalidTimeMesh : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid time mesh value.
     * @param N The invalid time mesh value received.
     */
    InvalidTimeMesh(int N) {
        msg = "Invalid value for time mesh, value must be positive, value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid spot mesh value is encountered.
 *
 * Spot mesh must be positive.
 */
class InvalidSpotMesh : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid spot mesh value.
     * @param N The invalid spot mesh value received.
     */
    InvalidSpotMesh(int N) {
        msg = "Invalid value for time mesh, value must be positive, value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid spot price is encountered.
 *
 * Spot price must be positive.
 */
class InvalidSpot : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid spot price.
     * @param N The invalid spot price received.
     */
    InvalidSpot(double N) {
        msg = "Invalid value for spot price, value must be positive, value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid volatility value is encountered.
 *
 * Volatility must be positive.
 */
class InvalidVolatility : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to initialize the error message with the invalid volatility value.
     * @param N The invalid volatility value received.
     */
    InvalidVolatility(double N) {
        msg = "Invalid value for volatility, value must be positive, value received: ";
        msg += std::to_string(N);
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

/**
 * @brief Exception thrown when an invalid time value is encountered.
 *
 * Time values are expected to be within valid bounds.
 */
class InvalidTime : public OptionExceptions {
    std::string msg;

public:
    /**
     * @brief Constructor to set the default error message for invalid time.
     */
    InvalidTime() {
        msg = "Value out of bounds";
    }

    /**
     * @brief Override the 'what()' method to return the custom error message.
     * @return The error message as a C-string.
     */
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};