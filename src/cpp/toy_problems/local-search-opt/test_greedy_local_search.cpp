#include <cassert>
#include <iostream>

#include "optimizer.h"
#include "optimizer.cpp"

template <typename T>
void TestGreedyLocalSearch(const std::vector<T>& domain, const std::function< double (T) >& score_f,
    const std::function< std::unordered_set<T> (T) >& neighborhood_f) {

    std::cout << std::endl << "Testing the GreedyLocalSearchOptimizer class." << std::endl << std::endl;

    utils::algorithms::GreedyLocalSearchOptimizer<T> greedy_opt(domain);

    assert(greedy_opt.StoppingCriteria(1) == false);
    assert(greedy_opt.StoppingCriteria(2) == false);
    assert(greedy_opt.StoppingCriteria(5) == true);
    assert(greedy_opt.StoppingCriteria(11) == true);

    auto init_val = greedy_opt.InitialValue();
    std::cout << "Starting solution value: " << init_val << std::endl;

    std::unordered_set<T> neighbors;
    for (const auto& el : domain) {
        neighbors.insert(el);
    }
    auto n = greedy_opt.ChooseNeighbor(neighbors);
    std::cout << "The chosen neighbor is: " << n << std::endl;

    std::cout << std::endl << "Starting the optimization process..." << std::endl;

    T final_soln = greedy_opt.Optimize(score_f, neighborhood_f);
    std::cout << "** The final solution is: " << final_soln << " **" << std::endl;
}

