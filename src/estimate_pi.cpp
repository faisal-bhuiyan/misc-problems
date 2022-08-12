// estimate_pi.cpp: Faisal Bhuiyan
// Description: Estimate the value of pi using a numerical method.

#include <cmath>
#include <iostream>

using namespace std;

double sum_of_series(size_t  n_terms) {
    double sum {0.};
    for (size_t i = 1; i <= n_terms; ++i) {
        sum += 1. / (i * i);
    }
    return sum;
}

int main() {
    int input_var = 0;

    cout << "Enter a number between 1 and 1000..." << "\n";
    cin >> input_var;

    double pi = sqrt(6. * sum_of_series(input_var));

    cout << "The estimated value of pi is: " << pi << "\n";

    return 0;
}

