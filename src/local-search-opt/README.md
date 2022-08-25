# local-search-optimization
This repository contains an object-oriented implementation for finding a general solution of the [local search-based optimization problem](https://en.wikipedia.org/wiki/Local_search_(optimization)). To this end, I implemented my proposed solution in C++.

## Instructions
From the root directory, compile and run the `main.cpp` program: `g++ main.cpp`.

## Problem Statement
Search problems are some of the most ubiquitous in computer science. In many instances there’s a notion of closeness or proximity between candidate solutions, which suggests local search algorithms/heuristics may be effective. We would like you to design a program to solve a general version of local search-based optimization.

Let $X$ be a (possibly infinite) set of candidates. Given an element $x ∈ X$, you are supplied with three black-box functions:

1. a score function $f : X → R+$, which assigns a real number $f(x)$ to every element $x ∈ X$;

2. a neighborhood function $n : X → 2^X$, where $2^X$ is the power set of $X$, i.e., the set of unordered tuples comprising elements of $X$, which represents the idea that element $x$ has a set of neighbors given by $n(x)$;

3. an equality function $eq : X^2 → {0, 1}$ which, given two elements $x, y ∈ X$, results in $eq(x, y) = 1$ if $x$ and $y$ are the same element, and $eq(x, y) = 0$ otherwise.

Given an initial candidate $x_0 ∈ X$, the task is to find $x$ such that $f(x)$ is minimized. Design and implement an algorithm or heuristic to solve this problem, then answer the following questions:

1. Is your procedure guaranteed to find a minimum? Global or local? If it is not guaranteed, are there additional assumptions that could be made about the input set $X$ that could provide that guarantee?

2. Say that, instead of minimizing a function $f(x)$, your goal is to find a path from the starting point to a desired point $x_k$, that is, a sequence of points $P = x_0, x_1, . . . x_k$ such that $x_{i+1} ∈ n(x_i)$. Design and implement an algorithm for solving this problem.

3. If we introduce a cost function $w : X^2 → R+$ such that $w(x, y)$ where $y ∈ n(x)$ is the cost of moving from $x$ to $y$, it is possible to assign a total cost to a given path $P$ by summing the costs for each “link”. How would you modify your algorithm from item $2$ to find a path $P$ with minimum cost? Is your algorithm guaranteed to find a correct answer? Why or why not?

## Solutions
1. A couple of optimization algorithms were implemented to find the solution to the proposed minimization problem - they are namely the `GreedyLocalSearchOptimizer` and the `SteepestDescentLocalSearchOptimizer` classes. The procedures are not guranteed to find a global minimum, however they work well for a local minimum. We need to make the additional assumption on the input set $X$ to be a convex function to guratee finding the global minimum.

2. To find the optimum path to the desired ending point, a new algorithm was developed inside the class `UniformCostPathFinder`. A steepest-descent based algorithm was implemented to find the shortest distance to the desired location based off a random starting point in the problem domain. The algorithm does not guarantee a solution, it depends greatly on the neighborhood function provided to the algorithm, among other things.

