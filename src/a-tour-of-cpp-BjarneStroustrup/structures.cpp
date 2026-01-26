// structures.cpp: Faisal Bhuiyan
// Description: Example code showing use of struct

#include <iostream>
#include <string>

// first version of the user-defined data type vector
struct Vector {
    int sz;        // number of elements
    double* elem;  // pointer to elements
};

// construct Vector
void vector_init(Vector& v, int s) {
    v.elem = new double[s];  // allocate an array of s doubles
    v.sz = s;
}

// a simple use of Vector:
// read s integers from cin and return their sum; s is assumed to be positive
double read_and_sum(int s) {
    Vector v;
    vector_init(v, s);  // allocate s elements for v

    std::cout << "Provide " << s << " integers in the console: \n";

    for (int i = 0; i != s; ++i) {
        std::cin >> v.elem[i];  // read into elements
    }

    double sum = 0;
    for (int i = 0; i != s; ++i) {
        sum += v.elem[i];  // compute the sum of the elements
    }
    return sum;
}

int main() {
    int n_elements = 3;
    auto sum = read_and_sum(n_elements);
    std::cout << "The sum of " << n_elements << " elements of the vector: " << sum << "\n";
}
