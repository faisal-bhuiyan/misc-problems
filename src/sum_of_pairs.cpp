// sum_of_pairs.cpp: Faisal Bhuiyan
// Description: How do you find all pairs of an integer array whose sum is equal to a given number?

#include <iostream>
#include <vector>

int main() {
    // inputs
    int desired_sum = 10;
    std::vector<int> input = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // solution: brute force
    // TODO Find more efficient solutions than going over all elements of the vector
    std::vector<std::pair<int, int>> pairs;

    for (int i : input) {
        for (int j : input) {
            if (auto sum = i + j; sum == desired_sum) {
                pairs.push_back(std::pair {i, j});
                std::cout << "pair: " << i << ", " << j  << "\n";
            }
        }
    }

    std::cout << "numer of pairs found: " << pairs.size() << "\n";
}

