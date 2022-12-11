// initialization.cpp: Faisal Bhuiyan
// Description: Examples of object initialization methods in C++

#include <complex>
#include <iostream>
#include <vector>

int main() {
    // Using the traditional, C-based initialization method based on =
    double d1 = 2.3;        // initialize d1 to 2.3
    std::cout << "value of d1: " << d1 << std::endl;

    // Using the initializer list method based of {}
    double d2 {2.3};        // initialize d2 to 2.3
    std::cout << "value of d2: " << d2 << std::endl;

    double d3 = {2.3};      // initialize d3 to 2.3 (the = is optional with {...})
    std::cout << "value of d3: " << d3 << std::endl;

    std::complex<double> z = 1.;    // a complex number with double-precision floating-point scalars
    std::cout << "value of z, real: " << z.real() << ", imaginary: " << z.imag() << std::endl;

    std::complex<double> z2 {d1, d2};
    std::cout << "value of z2, real: " << z2.real() << ", imaginary: " << z2.imag() << std::endl;

    std::vector<int> v {1, 2, 3, 4, 5};     // a vector of ints
    for (const auto& i : v) {
        std::cout << "the vector is: " << i << std::endl;
    }
}

