/*
 * Given an array of integers nums and an integer target, return indices of the two numbers such that
 * they add up to target.
 *
 * You may assume that each input would have exactly one solution, and you may not use the same
 * element twice.
 *
 * You can return the answer in any order.
 *
 * Example 1:
 * Input: nums = [2,7,11,15], target = 9
 * Output: [0,1]
 * Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].
 *
 * Example 2:
 * Input: nums = [3,2,4], target = 6
 * Output: [1,2]
 *
 * Example 3:
 * Input: nums = [3,3], target = 6
 * Output: [0,1]
 */

#include <unordered_map>
#include <vector>

/*
 * Commentary on the solution:
 * - This is a really classic hashmap problem given in interviews. I have seen this question come up
 *   in one interview myself.
 *
 * - The naive and brute force solution is to use two nested loops to check all pairs of numbers in
 * the array -> O(n^2) time, O(1) space.
 *
 * - We can do better than this by making the observation that we are trying to find a (key, value)
 * pair over a hashmap in this problem, where the key -> nums[index] and value -> index.
 * We can build the hashmap upfront at the cost of O(n) time and space complexity and then perform
 * a constant-time search for the presence of an element.
 *
 * - The smarter approach is to build the hashmap and search at the same time -> we can look for the
 * target in the existing hashmap by iterating over the elements of nums. If that index matches an
 * existing element in hashmap, our work is done. If not, we simply add it to the hashmap.
 *
 * Time Complexity: O(n)
 * - We iterate through the array once, and for each element, we perform a constant-time hashmap
 * lookup.
 *
 * Space Complexity: O(n)
 * - We use a hashmap to store the values and their indices. The space complexity is O(n) in the
 * worst case, where we need to store all elements in the hashmap.
 */

// version 1.01: Re-hashing the previous soln just for practice
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        // Classic hashmap problem given in interviews

        // We need to start by selecting the correct container for hashmap in C++
        std::unordered_map<int, int> hashmap{};

        // Now we need to create the hashmap - it is a map from value -> index
        // Rather than creating the hashmap upfront, we can be smart about it
        // by creating and searching for the target pair at the same time
        int needle{};
        for (size_t i = 0; i < nums.size(); ++i) {
            needle = target - nums[i];

            // see if we have this needle in the hashmap
            // if (hashmap.find(needle) != hashmap.end()) {
            if (hashmap.count(needle)) {
                // return the twosum pair indices - index of current number and index of its pair
                // from the hashmap
                return {static_cast<int>(i), hashmap[static_cast<size_t>(needle)]};
            }

            // needle does not exist, we need to add it to the hashmap
            hashmap[nums[i]] = i;
        }

        // could not find the twosum pair
        return {};
    }
};

// version 1.00: Solution from Neetcode video
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        // create a hash map to store value -> index of the vector nums
        std::unordered_map<int, int> index_map{};
        for (int index_i = 0; index_i < nums.size(); ++index_i) {
            // we are looking for following needle in nums
            auto needle = target - nums[index_i];
            // found the needle, let's return the indices
            if (auto search = index_map.find(needle); search != index_map.end()) {
                return {index_i, index_map[needle]};
            }
            // did not find needle, add element to hash map
            index_map[nums[index_i]] = index_i;
        }
        return {};
    }
};
