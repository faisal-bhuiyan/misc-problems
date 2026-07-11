/*
 * Given an array of integers nums which is sorted in ascending order, and an integer target, write a
 * function to search target in nums. If target exists, then return its index. Otherwise, return -1.

 * You must write an algorithm with O(log n) runtime complexity.
 *
 * Example 1:
 * Input: nums = [-1,0,3,5,9,12], target = 9
 * Output: 4
 * Explanation: 9 exists in nums and its index is 4
 *
 * Example 2:
 * Input: nums = [-1,0,3,5,9,12], target = 2
 * Output: -1
 * Explanation: 2 does not exist in nums so return -1
 */

// version 1.02: Binary search w/ two pointers -> time complexity O(log n) and space complexity O(1)
class Solution {
public:
    int search(const std::vector<int>& nums, int target) {
        // Degenerate case: nums is empty
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
        return -1;
    }
};

// version 1.01: Binary search w/ two pointers -> time complexity O(log n) and space complexity O(1)
class Solution {
public:
    int search(const std::vector<int>& nums, int target) {
        // Degenerate case: nums is empty
        if (nums.empty()) {
            return -1;
        }

        // Early exit case: nums in just one element
        if (nums.size() == 1) {
            return target == nums[0] ? 0 : -1;
        }

        // Binary search w/ two pointers
        auto low{0};                 // first elem
        auto high{nums.size() - 1};  // last elem
        int mid{0};
        int guess{0};
        while (low <= high) {
            mid = std::floor((low + high) / 2);
            guess = nums[mid];
            if (guess == target) {
                return mid;
            } else if (guess < target) {
                // guess was too low -> increase left pointer
                if (mid + 1 >= nums.size()) {
                    return -1;
                }
                low = mid + 1;
            } else {
                // guess was too high -> decrease right pointer
                if (mid - 1 < 0) {
                    return -1;
                }
                high = mid - 1;
            }
        }
        return -1;
    }
};
