#include <iostream>

double square(double x) {
    return x * x;
}

void print_square(double x) {
    std::cout << "The square of " << x << " is " << square(x) << "\n";
}

int main() {
    print_square(10.50);
}

