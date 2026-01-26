// unique_names.cpp: Faisal Bhuiyan
// Description: Find unique names from a couple of provided vector of strings.

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

/*
 * A simple function to find unique names from a couple of provided vector of strings
 */
std::vector<std::string> unique_names(
    const std::vector<std::string>& names1, const std::vector<std::string>& names2
) {
    std::set<std::string> unique_names;

    for (const auto& name : names1) {
        unique_names.insert(name);
    }

    for (const auto& name : names2) {
        unique_names.insert(name);
    }

    std::vector<std::string> result;

    for (const auto& name : unique_names) {
        result.push_back(name);
    }

    return result;
}

#ifndef RunTests
int main() {
    std::vector<std::string> names1 = {"Ava", "Emma", "Olivia"};
    std::vector<std::string> names2 = {"Olivia", "Sophia", "Emma"};

    std::vector<std::string> result{unique_names(names1, names2)};
    for (auto element : result) {
        std::cout << element << ' ';  // should print Ava Emma Olivia Sophia
    }
}
#endif
