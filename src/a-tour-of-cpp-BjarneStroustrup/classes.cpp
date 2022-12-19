// classes.cpp: Faisal Bhuiyan
// Description: Example code showing use of the primary user-defined construct in C++ - classes

#include <iostream>
#include <string>

/*
 * We often want to keep the representation inaccessible to users so as to ease use,
 * guarantee consistent use of the data, and allow us to later improve the represen-
 * tation. To achieve that, we have to distinguish between the interface to a type
 * (to be used by everyone) and its implementation (which has access to otherwise)
 * inaccessible data. The language mechanism for that in C++ is called a class.
 * A class has a set of members, which can be data, function, or type members. The
 * interface is defined by the public members of a class, and private members are
 * accessible only thru that interface.
 *
 * There is no fundamental difference between a struct and a class; a struct is simply
 * a class with members public by default. For example, you can define constructors and
 * other member functions for a struct.
*/

// second version of the user-defined data type Vector
class Vector {
public:
    // construct a vector
    Vector(int s): elem {new double[s]}, sz {s} {
    }

    // element access: subscripting
    inline double& operator[](size_t i) { return elem[i]; }

    inline int size() { return sz; }

private:
    int sz;          // number of elements
    double* elem;    // pointer to elements
};

/*
 * Basically the Vector object is a "handle" containing a pointer to the elements (elem)
 * and the number of elements (sz). The number of elements can vary from Vector object to
 * Vector object, and a Vector object itself can have a different number of elements at
 * different times. However, the Vector object itself is always the same size. This is the
 * basic technique for handling varying amounts of information in C++; a fixed-size handle
 * referring to a variable amount of data "elsewhere" (e.g. on the free store allocated by
 * the keyword "new").
*/

// A simplified version of read_and_sum() example from structures
double read_and_sum2(int s) {
    Vector v(s);

    std::cout << "Provide " << s << " integers in the console: \n";
    for (int i = 0; i != v.size(); ++i) {
        std::cin >> v[i]; // read into elements
    }

    double sum = 0;
    for (int i = 0; i != v.size(); ++i) {
        sum += v[i]; // compute the sum of the elements
    }
    return sum;
}

int main() {
    int n_elements = 3;
    auto sum = read_and_sum2(n_elements);
    std::cout << "The sum of " << n_elements << " elements of the vector: " << sum << "\n";
}

