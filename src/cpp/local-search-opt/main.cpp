#include <cassert>
#include <iostream>

#include "test_greedy_local_search.cpp"
#include "test_path_finder.cpp"
#include "test_steepest_descent_local_search.cpp"

int Score_Int_1(int x) {
    // A linearly increasing scoring scheme
    return x;
}

std::unordered_set<int> Neighborhood_Int_1(int x) {
    // Return the whole domain as the neighborhood
    return std::unordered_set<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
}

template <typename T>
bool Equal(T x, T y) {
    return x == y;
}

double Score_Double_1(double x) {
    // A linearly increasing scoring scheme
    return x;
}

std::unordered_set<double> Neighborhood_Double_1(double x) {
    // Return the whole domain as the neighborhood
    return std::unordered_set<double> { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };
}

double Score_Double_2(double x) {
    // A constant value for the score
    return 0.;
}

std::unordered_set<double> Neighborhood_Double_2(double x) {
    // Return the whole domain as the neighborhood
    return std::unordered_set<double> { -5., -4., -3., -2., -1., 0., 1., 2., 3., 4., 5. };
}

std::unordered_set<double> Neighborhood_Double_3(double x) {
    // Return the left and right elements
    if (x == -5.) {
        return std::unordered_set<double> { x+1 };
    } else if ( x == 5. ) {
        return std::unordered_set<double> { x-1 };
    } else {
        return std::unordered_set<double> { x-1, x+1 };
    }
}

// TODO Add unit tests for more primitive data types
// TODO Add unit tests for different neighborhood functions
// TODO Add unit tests for additional convex and non-convex functions to test the algorithms

int main() {
    // Create a domain with all integers in range [1, 10]
    std::vector<int> domain_int_1 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    // Create a domain with doubles in range [1., 10.] with increments of 1.
    std::vector<double> domain_double_1 { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10. };

    // Create a domain with doubles in range [-5., 5.] with increments of 1.
    std::vector<double> domain_double_2 { -5., -4., -3., -2., -1., 0., 1., 2., 3., 4., 5. };

    // *** Test the greedy local search algorithm: We may not reach the optimum value for
    // this method

    // Based off the provided arguments, the optimum value should be at 1
    TestGreedyLocalSearch<int>(domain_int_1, Score_Int_1, Neighborhood_Int_1);

    // Based off the provided arguments, the optimum value should be at 1.0
    TestGreedyLocalSearch<double>(domain_double_1, Score_Double_1, Neighborhood_Double_1);

    // Based off the provided arguments, the optimum value should be same as the initial_soln
    TestGreedyLocalSearch<double>(domain_double_2, Score_Double_2, Neighborhood_Double_2);

    // *** Test the steepest descent local search algorithm: We expect to reach the optimum value
    // for this method

    // Based off the provided arguments, the optimum value should be at 1
    TestSteepestDescentLocalSearch<int>(domain_int_1, Score_Int_1, Neighborhood_Int_1);

    // Based off the provided arguments, the optimum value should be at 1.0
    TestSteepestDescentLocalSearch<double>(domain_double_1, Score_Double_1, Neighborhood_Double_1);

    // Based off the provided arguments, the optimum value should be same as the initial_soln
    TestSteepestDescentLocalSearch<double>(domain_double_2, Score_Double_2, Neighborhood_Double_2);

    // Path finding problem:
    // Create a domain with doubles in range [-5., 5.] with increments of 1.
    // The desired ending point is 0 and we should reach there.
    double ending_pt {0.};
    std::vector<double> domain_double_3 { -5., -4., -3., -2., -1., 0., 1., 2., 3., 4., 5. };
    TestUniformCostPathFinder<double>(domain_double_3, ending_pt, Neighborhood_Double_3);
}

