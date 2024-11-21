#pragma once

#include <vector>
#include <iostream>
#include <iomanip>

class Lower {

    std::vector<double> subdiag_;
    std::vector<double> diag_;

public:
    Lower(std::vector<double> subdiag, std::vector<double> diag) : subdiag_(subdiag), diag_(diag) {}

    std::vector<double> solve(std::vector<double> b) {
        std::vector<double> x(b.size());
        x[0] = b[0] / diag_[0];
        for (size_t ii = 1; ii < b.size(); ii++) {
            x[ii] = (b[ii] - subdiag_[ii - 1] * x[ii - 1]) / diag_[ii];
        }
        return x;
    }
};

class Upper {

    std::vector<double> diag_;
    std::vector<double> superdiag_;

public:
    Upper(std::vector<double> diag, std::vector<double> superdiag) : diag_(diag), superdiag_(superdiag) {}

    std::vector<double> solve(std::vector<double> b) {
        int n = int(b.size());
        std::vector<double> x(n);
        x[n - 1] = b[n - 1] / diag_[n - 1];
        for (int ii = n - 2; ii >= 0; ii--) {
            x[ii] = (b[ii] - superdiag_[ii] * x[ii + 1]) / diag_[ii];
        }
        return x;
    }
};

class Tridiag {

    std::vector<double> subdiag_;
    std::vector<double> diag_;
    std::vector<double> superdiag_;

public:
    Tridiag() {}

    Tridiag(std::vector<double> subdiag, std::vector<double> diag, std::vector<double> superdiag);

    std::vector<double> operator*(std::vector<double> x);

    std::vector<double> solve(std::vector<double> b);

    size_t size() { return diag_.size(); }

    void display() {
        size_t n = diag_.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) {
                    std::cout << std::setw(8) << diag_[i] << " ";
                }
                else if (i == j + 1) {
                    std::cout << std::setw(8) << subdiag_[j] << " ";
                }
                else if (j == i + 1) {
                    std::cout << std::setw(8) << superdiag_[i] << " ";
                }
                else {
                    std::cout << std::setw(8) << "0.00" << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};