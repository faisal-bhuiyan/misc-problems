// count_x.cpp: Faisal Bhuiyan
// Description: An example to demonstrate utility of nullptr

#include <iostream>
#include <string.h>

int count_x(const char* p, char x) {
    // count the number of occurances x in p[]
    // p is assumed to point to a zero-terminated array of char (or to nothing)
    if (p == nullptr) {
        return 0;
    }

    int count = 0;
    while (*p) {
        if (*p == x) {
            count++;
        }
        ++p;
    }
    return count;
}

int main() {
    char str[] = {"hello world!"};
    char ch {'h'};
    auto n = count_x(str, ch);
    std::cout << "Number of times '" << ch << "' appears in \"" << str << "\" is: " << n << "\n";
}

