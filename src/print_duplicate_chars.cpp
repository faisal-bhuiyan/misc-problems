// print_duplicate_chars.cpp: Faisal Bhuiyan
// Description: How do you print duplicate characters from a string?

#include <algorithm>
#include <iostream>
#include <set>
#include <string>

std::vector<std::string::iterator> FindAll(std::string& s, char c) {
    std::vector<std::string::iterator> res;
    for (auto p = s.begin(); p!= s.end(); ++p) {
        if (*p == c) {
            res.push_back(p);
        }
    }
    return res;
}

int main () {
    // input
    std::string string1 = "hello world";
    std::string string2 = "bye world";

    std::string provided_string = string1;

    // Solution 1
    std::string tested;
    std::string duplicate;

    int count = 1;

    std::cout << "Using method 1..." << "\n";

    for (const auto& c : provided_string) {
        if (std::find(tested.begin(), tested.end(), c) == tested.end()) {
            if (std::find(provided_string.begin() + count, provided_string.end(), c) != provided_string.end()) {
                std::cout << "Duplicated character: " << c << "\n";
                duplicate.push_back(c);
            }
        }
        tested.push_back(c);
        count++;
    }
    std::cout << "Number of duplicate characters: " << duplicate.size() << "\n";

    // Solution 2
    // TODO Check for already tested character and skip them

    std::cout << "\n" << "Using method 2..." << "\n";

    for (const auto& c : provided_string) {
        auto res = FindAll(provided_string, c);

        if (res.size() > 1) {
            std::cout << "character: " << c << ", duplicates: " << res.size() << "\n";
        }
    }
}

