// separate_compilation_vector.cpp: Faisal Bhuiyan
// Description: Introduction to modularity in C++ (cpp file containing function implementations)

#include "separate_compilation_vector.h"  // get Vector's interface

/*
 * A cpp file that is compiled by itself (including the h files it #includes) is called a
 * "translation unit". A program consists of many thousand translation units.
 */

// Definition of the functions in Vector class

Vector::Vector(size_t s) : elem{new double[s]}, sz{s} {
}

double& Vector::operator[](size_t i) const {
    return this->elem[i];
}

size_t Vector::size() const {
    return this->sz;
}
