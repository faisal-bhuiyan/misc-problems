#pragma once

#include <functional>
#include <unordered_set>
#include <vector>

namespace utils {
namespace algorithms {

/// An abstract class to define the interface of an arbitrary optimization algorithm
template<typename T>
class Optimizer {
public:
    // We don't need a ctor for this abstract class, but a dtor is necessary
    virtual ~Optimizer() = default;

    // Let's add some pure virtual functions to facilitate interface inheritance
    virtual T InitialValue() const = 0;
    virtual T ChooseNeighbor(const std::unordered_set<T>&) const = 0;
    virtual bool StoppingCriteria(int iterations) const = 0;
    virtual T Optimize(const std::function< double (T) >&, const std::function< std::unordered_set<T> (T) >&) const = 0;
};


/* Implement a greedy local search optimization algorithm - it is a fast method of doing a local search,
 * finding the optimum solution is not guaranteed even for convex problems. The speed of this technique
 * comes from the fact that only one neighbor is evaluated at random to take the next step.
 */
template<typename T>
class GreedyLocalSearchOptimizer : public Optimizer<T> {
public:
    GreedyLocalSearchOptimizer(const std::vector<T>&);

    ~GreedyLocalSearchOptimizer() = default;

    /// Generates a random intial candidate, i.e. starting solution x_0 from the domain
    T InitialValue() const override;

    /// Chooses a random member from the neighbors
    T ChooseNeighbor(const std::unordered_set<T>&) const override;

    /// Provides a stopping criteria for the algorithm
    bool StoppingCriteria(int iterations) const override;

    /// Finds an optimum solution within the domain for the provided score, neighborhood, and equality functions
    T Optimize(const std::function< double (T) >&, const std::function< std::unordered_set<T> (T) >&) const override;

protected:
    std::vector<T> domain_;
};


/* Implement a steepest descent-based local search optimization algorithm - it is more expensive than the
 * greedy search method since all the neighbors of the current solution are evaluated for the best solution
 * for the next iteration.
 */
template<typename T>
class SteepestDescentLocalSearchOptimizer : public GreedyLocalSearchOptimizer<T> {
public:
    using GreedyLocalSearchOptimizer<T>::GreedyLocalSearchOptimizer;

    /// Provides a stopping criteria for the algorithm
    bool StoppingCriteriaDelta(double next, double current) const;

    /// Finds an optimum solution within the domain for the provided score, neighborhood, and equality functions
    T Optimize(const std::function< double (T) >&, const std::function< std::unordered_set<T> (T) >&) const override;
};

// TODO Implement restarts & simulated annealing local search optimization to improve performance for problems
// with lots of local minimas and maximas (i.e. non-convex problems)

/* Implement a steepest descent-based path finding algorithm - our goal is to find a path from the starting point
 * to a desired ending point.
 */
template<typename T>
class UniformCostPathFinder : public SteepestDescentLocalSearchOptimizer<T> {
public:
    UniformCostPathFinder(const std::vector<T>&, T);

    /// Finds the distance between two elements of the search domain
    double Distance(const T& el1, const T& el2) const;

    /// Finds an optimum solution
    T OptimumPath(const std::function< std::unordered_set<T> (T) >&) const;

private:
    T ending_pt_;
};


} // namespace algorithms
} // namespace utils

