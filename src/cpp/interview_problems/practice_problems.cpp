#include <cmath>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
 * Most important problems for interviews:
 *
 * *** TwoSum: Creating hashmap (std::unordered_map) to keep track of {value -> index} mapping in an
 * array
 *
 * *** Fibonacci: Incremental solution vs. recursive solution to get the n-th value of the Fibonacci
 * series
 *
 * *** Linked list reversal: reverse a linked list (incremental solution vs. recursive solution)
 *
 * *** Merge intervals: merge overlapping intervals in a list of intervals -> sorting and then
 * merging 1D intervals
 *
 * *** DFS/BFS: depth-first search/breadth-first search -> Core graph traversal algorithms
 *
 * ** Binary/bisection search: find the index of the target in the sorted vector nums -> two pointers
 * technique
 *
 * * sqrt(x): find the integer square root of a given integer x -> Newton's method to get quadratic
 * solution
 *
 * * Palindrome: check if a vector reads the same forwards and backwards -> two pointers technique
 *
 * * Rotate matrix: rotate an n x n matrix 90 degrees clockwise in-place -> transpose and then
 * reflect
 */

std::vector<int> flatten(const std::vector<std::vector<int>>& nested) {
    std::vector<int> res{};
    for (const auto& vec : nested) {
        for (const auto elem : vec) {
            res.push_back(elem);
        }
    }
    return res;
}

std::vector<int> runningSum(const std::vector<int>& nums) {
    // no elements in sum
    if (nums.empty()) {
        return {};
    }

    // Construct the running sum vector
    std::vector<int> res(nums.size(), 0);
    res[0] = nums[0];
    for (size_t i{1}; i < nums.size(); ++i) {
        res.push_back(res[i - 1] + nums[i]);
    }
    return res;
}

//---------------------------------------------------------------------------
// Binary search
//---------------------------------------------------------------------------
// Binary search: find the index of the target in the sorted vector nums
int binarySearch(const std::vector<int>& nums, int target) {
    // nums is empty
    if (nums.empty()) {
        return -1;
    }

    // Binary search w/ two pointers
    int low{0};                                   // first elem
    int high{static_cast<int>(nums.size()) - 1};  // last elem
    int mid{0};
    int guess{0};
    while (low <= high) {
        mid = (low + high) / 2;
        guess = nums[mid];
        if (guess == target) {
            return mid;
        } else if (guess < target) {
            // guess was too low -> increase left pointer
            low = mid + 1;
        } else {
            // guess was too high -> decrease right pointer
            high = mid - 1;
        }
    }
    // target is not present within nums
    return -1;
}

int findMax(const std::vector<int>& nums) {
    // assumption: vector is non-empty
    int max_elem{nums[0]};
    for (auto num : nums) {
        if (num > max_elem) {
            max_elem = num;
        }
    }
    return max_elem;
}

double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double dot_prod{};
    for (size_t i{0}; i < a.size(); ++i) {
        dot_prod += a[i] * b[i];
    }
    return dot_prod;
}

bool isPrime(int n) {
    // n = 1
    if (n == 1) {
        return false;
    }

    // n = 2
    if (n == 2) {
        return true;
    }

    // even numbers
    if (n % 2 == 0) {
        return false;
    }

    int limit = static_cast<int>(std::sqrt(n));
    for (int i = 3; i <= limit; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
// TwoSum
//---------------------------------------------------------------------------
std::pair<int, int> twoSum(const std::vector<int>& nums, int target) {
    // This is a classic hashmap problem -> in C++ we need std::unordered_map
    std::unordered_map<int, int> hashmap{};
    for (size_t i = 0; i < nums.size(); ++i) {
        // This is the value we are looking for
        int needle{target - nums[i]};
        // If this exists in the hashmap, we return with the indices of two numbers
        if (hashmap.find(needle) != hashmap.end()) {
            return {i, hashmap[needle]};
        }
        // Needle is not present in hashmap -> let's add it
        hashmap[nums[i]] = i;
    }
    return {};
}

//---------------------------------------------------------------------------
// Integer square root
//---------------------------------------------------------------------------
int isqrt(int n) {
    if (n == 0) {
        return 0;
    }

    // nearest integer square root
    int sq_root{1};
    for (size_t i = 2; static_cast<long long>(i * i) <= n; ++i) {
        sq_root = i;
    }
    return sq_root;
}

//---------------------------------------------------------------------------
// Fibonacci
//---------------------------------------------------------------------------
// Calculate the n-th value of Fibonacci series - incremental soln O(n) time complexity and O(1)
// space complexity
//  int fibonacci(int n) {
//     // base cases
//     if (n <= 0) {
//         return 0;
//     }
//     if (n == 1) {
//         return 1;
//     }

//     // incremental case
//     int fib_current{1};
//     int fib_prev{0};
//     for (int i = 2; i <= n; ++i) {
//         int temp = fib_current;
//         fib_current += fib_prev;
//         fib_prev = temp;
//     }
//     return fib_current;
// }

// Fibonacci series - recursive soln O(2^n) time complexity
int fibonacci(int n) {
    // base case
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    // recursive case
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    struct TestCase {
        int input;
        int expected;
    };

    std::vector<TestCase> tests = {
        {0, 0},                                                      // base case
        {1, 1},                                                      // base case
        {2, 1},                                                      // first non-trivial
        {3, 2}, {4, 3}, {5, 5}, {6, 8}, {7, 13}, {10, 55}, {-1, 0},  // negative input
    };

    int passed = 0;
    for (size_t i = 0; i < tests.size(); ++i) {
        int result = fibonacci(tests[i].input);
        bool ok = (result == tests[i].expected);
        std::cout << "Test " << i + 1 << (ok ? " PASSED" : " FAILED") << " (got " << result
                  << ", expected " << tests[i].expected << ")\n";
        if (ok)
            ++passed;
    }

    std::cout << passed << "/" << tests.size() << " tests passed\n";
    return 0;
}

//---------------------------------------------------------------------------
// First duplicate
//---------------------------------------------------------------------------
// Return the first element that appears more than once. If no duplicate exists, return -1.
int firstDuplicate(const std::vector<int>& nums) {
    // We don't need a map here since index is not required just the value -> use unordered set
    std::unordered_set<int> seen{};
    for (size_t i{0}; i < nums.size(); ++i) {
        int needle{nums[i]};
        // if the element is present in seen -> return it
        if (seen.count(needle)) {
            return needle;
        }
        // else add it to the map
        seen.insert(needle);
    }
    return -1;
}

// int main() {
//     struct TestCase {
//         std::vector<int> input;
//         int expected;
//     };

//     std::vector<TestCase> tests = {
//         {{3, 1, 4, 1, 5, 3}, 1},  // 1 appears before 3
//         {{1, 2, 3}, -1},          // no duplicates
//         {{}, -1},                 // empty
//         {{2, 2}, 2},              // duplicate at start
//         {{1, 2, 3, 4, 3}, 3},     // duplicate at end
//     };

//     int passed = 0;
//     for (size_t i = 0; i < tests.size(); ++i) {
//         int result = firstDuplicate(tests[i].input);
//         bool ok = (result == tests[i].expected);
//         std::cout << "Test " << i + 1 << (ok ? " PASSED" : " FAILED") << " (got " << result
//                   << ", expected " << tests[i].expected << ")\n";
//         if (ok)
//             ++passed;
//     }

//     std::cout << passed << "/" << tests.size() << " tests passed\n";
//     return 0;
// }

//---------------------------------------------------------------------------
// Palindrome
//---------------------------------------------------------------------------
// Return true if the vector reads the same forwards and backwards.
bool isPalindrome(const std::vector<int>& nums) {
    // empty vector
    if (nums.empty()) {
        return true;
    }

    // this smells like a two pointer problem
    auto left{0};
    auto right{nums.size() - 1};
    while (left < right) {
        if (nums[left] != nums[right]) {
            return false;
        }
        ++left;
        --right;
    }
    return true;
}

// int main() {
//     struct TestCase {
//         std::vector<int> input;
//         bool expected;
//     };

//     std::vector<TestCase> tests = {
//         {{1, 2, 3, 2, 1}, true},   // odd length palindrome
//         {{1, 2, 2, 1}, true},      // even length palindrome
//         {{1, 2, 3, 4, 5}, false},  // not a palindrome
//         {{1}, true},               // single element
//         {{}, true},                // empty
//         {{1, 2}, false},           // two different elements
//         {{5, 5}, true},            // two same elements
//     };

//     int passed = 0;
//     for (size_t i = 0; i < tests.size(); ++i) {
//         bool result = isPalindrome(tests[i].input);
//         bool ok = (result == tests[i].expected);
//         std::cout << "Test " << i + 1 << (ok ? " PASSED" : " FAILED") << " (got " << result
//                   << ", expected " << tests[i].expected << ")\n";
//     }
//     return 0;
// }

//---------------------------------------------------------------------------
// Missing number
//---------------------------------------------------------------------------
// Given a vector containing n distinct numbers in the range [0, n], return the one number that is
// missing.
// Hint: think about what the sum of [0, n] should be.
int missingNumber(const std::vector<int>& nums) {
    // Sum of all numbers from 0 -> n should be n(n+1) / 2
    int nums_sum{std::accumulate(nums.begin(), nums.end(), 0)};
    int nums_size{static_cast<int>(nums.size())};
    int expected_sum{nums_size * (nums_size + 1) / 2};
    return expected_sum - nums_sum;
}

// int main() {
//     struct TestCase {
//         std::vector<int> input;
//         int expected;
//     };

//     std::vector<TestCase> tests = {
//         {{3, 0, 1}, 2},                    // general case
//         {{0, 1}, 2},                       // missing last
//         {{9, 6, 4, 2, 3, 5, 7, 0, 1}, 8},  // larger input
//         {{0}, 1},                          // single element, missing 1
//         {{1}, 0},                          // single element, missing 0
//         {{0, 1, 2, 3, 4}, 5},              // missing last in sequence
//         {{1, 2, 3, 4, 5}, 0},              // missing 0
//     };

//     int passed = 0;
//     for (size_t i = 0; i < tests.size(); ++i) {
//         int result = missingNumber(tests[i].input);
//         bool ok = (result == tests[i].expected);
//         std::cout << "Test " << i + 1 << (ok ? " PASSED" : " FAILED") << " (got " << result
//                   << ", expected " << tests[i].expected << ")\n";
//         if (ok)
//             ++passed;
//     }

//     std::cout << passed << "/" << tests.size() << " tests passed\n";
//     return 0;
// }

//---------------------------------------------------------------------------
// Rotate matrix
//---------------------------------------------------------------------------
// Rotate an n x n matrix 90 degrees clockwise in-place
// Hint: think transpose first, then reflect
void rotateMatrix(std::vector<std::vector<int>>& matrix) {
    // Guard against empty matrix
    if (matrix.empty()) {
        return;
    }

    // Step 1: transpose matrix
    size_t nums_rows{matrix.size()};
    size_t nums_cols{matrix[0].size()};
    for (size_t i{0}; i < nums_rows; ++i) {
        // modify only upper-triangle
        for (size_t j{i + 1}; j < nums_cols; ++j) {
            // swap off-diagonal members
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }

    // Step 2: Reflect matrix
    for (size_t i{0}; i < nums_rows; ++i) {
        // modify only up to 1 less than middle col
        for (size_t j{0}; j < nums_cols / 2; ++j) {
            // swap off-diagonal members
            std::swap(matrix[i][j], matrix[i][nums_cols - 1 - j]);
        }
    }
}

// void printMatrix(const std::vector<std::vector<int>>& matrix) {
//     for (const auto& row : matrix) {
//         for (const auto& val : row) {
//             std::cout << val << " ";
//         }
//         std::cout << "\n";
//     }
// }

// bool matricesEqual(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b)
// {
//     return a == b;
// }

// int main() {
//     struct TestCase {
//         std::vector<std::vector<int>> input;
//         std::vector<std::vector<int>> expected;
//     };

//     std::vector<TestCase> tests = {
//         {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, {{7, 4, 1}, {8, 5, 2}, {9, 6, 3}}},  // 3x3
//         {{{1, 2}, {3, 4}}, {{3, 1}, {4, 2}}},                                    // 2x2
//         {{{1}}, {{1}}},                                                          // 1x1
//         {{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}},
//          {{13, 9, 5, 1}, {14, 10, 6, 2}, {15, 11, 7, 3}, {16, 12, 8, 4}}},  // 4x4
//     };

//     int passed = 0;
//     for (size_t i = 0; i < tests.size(); ++i) {
//         rotateMatrix(tests[i].input);
//         bool ok = matricesEqual(tests[i].input, tests[i].expected);
//         std::cout << "Test " << i + 1 << (ok ? " PASSED" : " FAILED") << "\n";
//         if (!ok) {
//             std::cout << "  Got:\n";
//             printMatrix(tests[i].input);
//             std::cout << "  Expected:\n";
//             printMatrix(tests[i].expected);
//         }
//         if (ok)
//             ++passed;
//     }

//     std::cout << passed << "/" << tests.size() << " tests passed\n";
//     return 0;
// }

//---------------------------------------------------------------------------
// Pascal's Triangle
//---------------------------------------------------------------------------

// Let's try an iterative soln first
class Solution {
public:
    vector<vector<int>> generate(int numRows) {
        // base case
        if (numRows == 0) {
            return {};
        }
        if (numRows == 1) {
            return {{1}};
        }

        // iterative case
        vector<vector<int>> pascals_triangle{{1}};
        for (size_t i = 1; i < numRows; ++i) {
            // compute the next row in pascals triangle
            vector<int> row(i + 1);
            // first and last elements of each row are always 1
            row[0] = 1;
            row[i] = 1;
            for (size_t j = 1; j < i; ++j) {
                // each element is the sum of the two elements above it
                row[j] = pascals_triangle[i - 1][j - 1] + pascals_triangle[i - 1][j];
            }
            pascals_triangle.emplace_back(row);
        }
        return pascals_triangle;
    }
};
