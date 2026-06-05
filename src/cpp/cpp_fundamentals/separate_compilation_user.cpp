// separate_compilation_user.cpp: Faisal Bhuiyan
// Description: Introduction to modularity in C++ (cpp file containing a user function)

#include <cmath>

#include "separate_compilation_vector.h"  // get Vector's interface

// An example function that uses the Vector class to calculate the sum of square roots
// of the members
double sqrt_sum(const Vector& v) {
    double sum{0.};
    for (size_t i = 0; i != v.size(); ++i) {
        sum += std::sqrt(v[i]);
    }
    return sum;
}

int main() {
    std::cout << "I currently do nothing \n";
}
