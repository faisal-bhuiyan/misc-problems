// constants.cpp: Faisal Bhuiyan
// Description: Demonstration of immutability in C++ with code snippets

#include <cmath>
#include <iostream>
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

    constexpr int dmv{17};  // dmv is a named constant
    std::cout << "value of the constexpr dmv: " << dmv << "\n";
    int var{17};  // var is not a constant
    std::cout << "value of the non-const var: " << var << "\n";
    const double sqv = std::sqrt(var);  // sqv is a named constant possibly computed @run time
    std::cout << "value of the const double sqv (square root of var): " << sqv << "\n";

    double sum(const std::vector<double>&);  // sum will not modify its argument

    std::vector<double> v{1.2, 3.4, 4.6};  // v is not a constant
    std::cout << "vector v: {1.2, 3.4, 4.6}\n";
    const double s1 = sum(v);  // OK: sum(v) is evaluated @run time
    std::cout << "value of the const double s1 (sum of vector v): " << s1 << "\n";
    // constexpr double s2 = sum(v); // error: sum(v) is not a constant expression
}

double sum(const std::vector<double>& v) {
    double sum{0.};
    for (double d : v) {
        sum += d;
    }
    return sum;
}
