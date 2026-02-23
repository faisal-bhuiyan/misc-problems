#include <cassert>
#include <iostream>

#include "optimizer.h"
#include "optimizer.cpp"

template <typename T>
void TestUniformCostPathFinder(const std::vector<T>& domain, const T& ending_pt,
    const std::function< std::unordered_set<T> (T) >& neighborhood_f) {

    std::cout << std::endl << "Testing the UniformCostPathFinder class." << std::endl;

    utils::algorithms::UniformCostPathFinder<double> path_opt(domain, ending_pt);

    std::cout << std::endl << "Starting the path finding process..." << std::endl;

    double final_soln = path_opt.OptimumPath(neighborhood_f);
    std::cout << "** The final solution is: " << final_soln << " **" << std::endl;
}

