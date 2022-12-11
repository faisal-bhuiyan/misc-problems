// constants.cpp: Faisal Bhuiyan
// Description: Demonstration of immutability in C++

#include <iostream>
#include <cmath>
#include <vector>

int main() {
    /*
     * C++ supports two notions of immutability:
     * - const: meaning roughly "I promise not to change this value."
     * This is used primarily to specify interfaces so that data can be passed to
     * functions without fear of being modified. The compiler enforces the promise
     * made by const. The value of const can be calculated @ run time.
     *
     * - constexpr: meaning roughly "to be evaluated @ compile time." This is used
     * primarily to specify constants, to allow placement of data in read-only memory,
     * and for performance. The value of constexpr must be calculated by the compiler.
     */

    constexpr int dmv {17};     // dmv is a named constant
    int var {17};    // var is not a constant
    const double sqv = std::sqrt(var);    // sqv is a named constant possibly computed @run time

    double sum(const std::vector<double>&);    // sum will not modify its argument

    std::vector<double> v {1.2, 3.4, 4.6};    // v is not a constant
    // const double s1 = sum(v);    // OK: sum(v) is evaluated @run time
    // constexpr double s2 = sum(v);    // error: sum(v) is not a constant expression
}

