#include <cassert>
#include <iostream>

#include "optimizer.h"
#include "optimizer.cpp"

template <typename T>
void TestSteepestDescentLocalSearch(const std::vector<T>& domain, const std::function< double (T) >& score_f,
    const std::function< std::unordered_set<T> (T) >& neighborhood_f) {

    std::cout << std::endl << "Testing the SteepestDescentLocalSearchOptimizer class." << std::endl << std::endl;

    utils::algorithms::SteepestDescentLocalSearchOptimizer<T> steepest_des(domain);

    assert(steepest_des.StoppingCriteria(1) == false);
    assert(steepest_des.StoppingCriteria(2) == false);
    assert(steepest_des.StoppingCriteria(7) == true);

    auto init_val_2 = steepest_des.InitialValue();
    std::cout << "Starting solution value: " << init_val_2 << std::endl;

    T final_soln = steepest_des.Optimize(score_f, neighborhood_f);
    std::cout << "** The final solution is: " << final_soln << " **" << std::endl;
}

