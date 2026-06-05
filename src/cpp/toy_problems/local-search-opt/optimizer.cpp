#pragma once

#include "optimizer.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace utils {
namespace algorithms {

template<typename T>
GreedyLocalSearchOptimizer<T>::GreedyLocalSearchOptimizer(const std::vector<T>& domain) :
    domain_(domain) {
}

template <typename T>
T GreedyLocalSearchOptimizer<T>::InitialValue() const {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, this->domain_.size() - 1);
    int rvalue = uniform_dist(r);

    // std::cout << "rvalue: " << rvalue << std::endl;

    return this->domain_[rvalue];
}

template <typename T>
T GreedyLocalSearchOptimizer<T>::ChooseNeighbor(const std::unordered_set<T>& neighbors) const {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, this->domain_.size() - 1);
    int rvalue = uniform_dist(r);

    auto it = neighbors.cbegin();
    std::advance(it, rvalue);

    return *it;
}

template <typename T>
bool GreedyLocalSearchOptimizer<T>::StoppingCriteria(int iterations) const {
    if (iterations > static_cast<int>(0.25 * this->domain_.size())) {
        return true;
    }

    return false;
}

template <typename T>
T GreedyLocalSearchOptimizer<T>::Optimize(const std::function< double (T) >& score_f,
    const std::function< std::unordered_set<T> (T) >& neighborhood_f) const {

    // A basic local search minimization algorithm - here we just run with a randomly selected negihbor
    // for the next solution
    T current_soln = this->InitialValue();

    int iterations {0};

    while (!this->StoppingCriteria(iterations)) {
        std::cout << "iteration number: " << iterations <<
            " , current solution = " << current_soln;

        auto neighbors = neighborhood_f(current_soln);
        auto next_soln = ChooseNeighbor(neighbors);

        std::cout << ", next solution (proposed): " << next_soln << std::endl;

        if (score_f(next_soln) < score_f(current_soln)) {
            current_soln = next_soln;
        }

        iterations++;
    }

    return current_soln;
}

template <typename T>
bool SteepestDescentLocalSearchOptimizer<T>::StoppingCriteriaDelta(double next, double current) const {
    if (next < current) {
        return false;
    }
    return true;
}

template <typename T>
T SteepestDescentLocalSearchOptimizer<T>::Optimize(const std::function< double (T) >& score_f,
    const std::function< std::unordered_set<T> (T) >& neighborhood_f) const {

    // A steepest descent local search minimization algorithm - here we look for the best soln
    // among all neighbors of the current soln
    T current_soln = this->InitialValue();
    int iterations {0};
    double score_current_soln {0.};
    double score_next_soln {0.};

    do {
        std::cout << "iteration number: " << iterations <<
            ", current solution: " << current_soln;

        auto neighbors = neighborhood_f(current_soln);

        std::vector<double> neighbor_results;
        for (const auto & n : neighbors) {
            neighbor_results.push_back(score_f(n));
        }

        auto result = std::min_element(neighbor_results.begin(), neighbor_results.end());
        auto it = neighbors.cbegin();
        std::advance(it, std::distance(neighbor_results.begin(), result));

        T next_soln = *it;

        std::cout << ", next solution (proposed): " << next_soln << std::endl;

        score_current_soln = score_f(current_soln);
        score_next_soln = score_f(next_soln);

        if (score_next_soln <= score_current_soln) {
            current_soln = next_soln;
        }

        iterations++;
    } while (!this->StoppingCriteriaDelta(score_next_soln, score_current_soln));

    return current_soln;
}

template <typename T>
UniformCostPathFinder<T>::UniformCostPathFinder(const std::vector<T>& domain, T ending_pt) :
    SteepestDescentLocalSearchOptimizer<T>(domain), ending_pt_(ending_pt) {
}

// TODO Explore some other methods of calculating distance that might be more appropriate
template <typename T>
double UniformCostPathFinder<T>::Distance(const T& el_1, const T& el_2) const {
    // std::cout << std::endl << "el_1: " << el_1 << ", el_2: " << el_2;

    auto it_1 = std::find(begin(this->domain_), end(this->domain_), el_1);
    auto it_2 = std::find(begin(this->domain_), end(this->domain_), el_2);

    // std::cout << ", Distance: " << std::abs(std::distance(it_1, it_2)) << std::endl;

    // TODO Handle exception that might arise here
    return std::abs(std::distance(it_1, it_2));
}

template <typename T>
T UniformCostPathFinder<T>::OptimumPath(const std::function< std::unordered_set<T> (T) >& neighborhood_f) const {

    // A steepest descent local search minimization algorithm - here we look for the best soln
    // among all neighbors of the current soln
    T current_soln = this->InitialValue();
    int iterations {0};
    double distance_current_soln {0.};
    double distance_next_soln {0.};

    std::cout << "We want to reach the ending point: " << this->ending_pt_ << std::endl;

    do {
        std::cout << "iteration number: " << iterations <<
            ", current solution: " << current_soln;

        auto neighbors = neighborhood_f(current_soln);

        std::vector<double> neighbor_results;
        for (const auto & n : neighbors) {
            neighbor_results.push_back(this->Distance(n, this->ending_pt_));
        }

        auto result = std::min_element(neighbor_results.begin(), neighbor_results.end());
        auto it = neighbors.cbegin();
        std::advance(it, std::distance(neighbor_results.begin(), result));

        T next_soln = *it;

        std::cout << ", next solution (proposed): " << next_soln << std::endl;

        distance_current_soln = this->Distance(current_soln, this->ending_pt_);
        distance_next_soln = this->Distance(next_soln, this->ending_pt_);

        if (distance_next_soln <= distance_current_soln) {
            current_soln = next_soln;
        }

        iterations++;
    } while (!this->StoppingCriteriaDelta(distance_next_soln, distance_current_soln));

    return current_soln;
}

} // namespace algorithms
} // namespace utils

