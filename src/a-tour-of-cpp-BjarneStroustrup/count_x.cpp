// count_x.cpp: Faisal Bhuiyan
// Description: An example to demonstrate the utility of nullptr

#include <iostream>
#include <string.h>

/*
 * nullptr in C++:
 * We try to ensure that a pointer always points to an object so that dereferencing
 * it is valid. When we don't have an object to point to or if we need to represent
 * the notion of "no object available" (e.g. for the end of a list), we give the
 * pointer the value nullptr ("the null pointer"). There is only one nullptr shared
 * by all pointer types.
 */

int count_x(const char* p, char x) {
    // It is often wise to check that a ponter argument actually points to something:
    // count the number of occurances of x in p[]
    // p is assumed to point to a zero-terminated array of char (or to nothing)
    if (p == nullptr) {
        return 0;
    }

    int count {0};
    // Loop will execute as long as p is not a nullptr
    while (*p) {
        if (*p == x) {
            // We can advance a pointer to point to the next element of an array using ++
            count++;
        }
        ++p;
    }

    /*
     * A test of a numeric value (e.g. while (*p) in count_x()) is equivalent to comparing
     * the value to 0 (e.g. while (*p != 0)). A test of a pointer value (e.g. if (p)) is
     * equivalent to comparing the value to nullptr (e.g. if (p != nullptr))
     */

    return count;
}

int main() {
    char str[] = {"hello world!"};
    char ch {'h'};
    auto n = count_x(str, ch);
    std::cout << "Number of times '" << ch << "' appears in \"" << str << "\" is: " << n << "\n";
}

