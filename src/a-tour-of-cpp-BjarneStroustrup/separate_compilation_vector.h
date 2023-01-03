// separate_compilation_vector.h: Faisal Bhuiyan
// Description: Introduction to modularity in C++ (header file containing function declarations)

#include <iostream>

/*
 * A key concept in C++ is the distinction between the interface to a part and its implementation.
 * At the langugage level, C++ represents interfaces by declarations. A declaration specifies all
 * that's needed to use a function or a type (e.g. a class). The critical idea here is that the
 * function bodies, i.e. the function definitions, can be "elsewhere" than the function
 * declarations.
 *
 * C++ supports a notion of separate compilation whereby user code sees only the declarations of
 * the types and functions used. The defs of those types and functions are in separate source
 * files and are compiled separately. This can be used to organize a program into a set of semi-
 * independent code fragments. Such separation can be used to minimize compilation times and to
 * strictly enforce separation of logically distinct parts of a program and thus minimizing the
 * chance of errors.
 *
 * Separate compilation is not a language issues, rather it is an issue of how best to take
 * advantage of a particular language implementation. The best approach to program organization
 * is to think of the program as a set of modules with well-defned dependencies, represent that
 * modularity logically through language features, and then exploit the modularity physically
 * through files for effective separate compilation.
 */

/// Declaration of a bare-bones Vector class
class Vector {
public:
    // construct a vector
    Vector(size_t s);

    // element access: subscripting
    double& operator[](size_t i) const;

    // return the size of the vector
    size_t size() const;

private:
    size_t sz;       // number of elements
    double* elem;    // pointer to elements
};

