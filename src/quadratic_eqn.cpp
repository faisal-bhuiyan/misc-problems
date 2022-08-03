// quadratic_eqn.cpp: Faisal Bhuiyan
// Description: Solve a quadratic equation

#include <cmath>
#include <iostream>
#include <string>

// TODO Extend logic to find imaginary roots as well
std::pair<double, double> findRoots(double a, double b, double c) {
    // roots = ( -b +- (b^2 - 4ac) ) / 2a
    double sq_root = std::sqrt(b * b - 4 * a *c);
    double root1 = (-b + sq_root) / (2 * a);
    double root2 = (-b - sq_root) / (2 * a);

    return std::pair { root1, root2 };
}

#ifndef RunTests
int main() {
    std::pair<double,double> roots = findRoots(2, 10, 8);
    std::cout << "Roots: " + std::to_string(roots.first) + ", " + std::to_string(roots.second) + "\n" ;
}
#endif

