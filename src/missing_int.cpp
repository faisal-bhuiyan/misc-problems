// missing_int.cpp: Faisal Bhuiyan
// Description: How do you find the missing number in a given integer array of 1 to 100?

#include <algorithm>
#include <iostream>

int main() {
    std::vector<int> arr(100);

    for (int i = 0; i < 100; i++) {
        arr[i] = i + 1;
    }

    // Select a member to be zero
    arr[13] = 0;

    for (int el : arr) {
        std::cout << el << std::endl;
    }

    // Solution - assuming only 1 int is missing
    for (int i = 1; i<= 100; i++) {
        auto member = std::find(arr.begin(), arr.end(), i);

        if (member == arr.end()) {
            std::cout << "missing int: " << i << std::endl;
            break;
        }
    }
}

