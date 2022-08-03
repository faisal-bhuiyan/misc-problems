// anagrams.cpp: Faisal Bhuiyan
// Description: How do you check if two strings are anagrams of each other?

#include <algorithm>
#include <iostream>
#include <string>

bool compare_strings(std::string s1, std::string s2) {
    if (s1.size() != s2.size()) {
        return false;
    }

    std::sort(s1.begin(), s1.end());
    std::sort(s2.begin(), s2.end());

    return s1 == s2 ? true : false;
}

int main() {
    std::string string_1 = "motherinlaw";
    std::string string_2 = "womanhitler";

    auto result = compare_strings(string_1, string_2);

    if (result) {
        std::cout << "Provided strings " << string_1 << ", " << string_2 << " are anagrams!" << "\n";
        return 0;
    }

    std::cout << "Provided strings " << string_1 << ", " << string_2 << " are NOT anagrams." << "\n";

    return 0;
}

