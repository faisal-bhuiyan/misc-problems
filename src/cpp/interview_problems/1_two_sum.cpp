/*
 * Given an array of integers nums and an integer target, return indices of the two numbers such that
 * they add up to target.

 * You may assume that each input would have exactly one solution, and you may not use the same
 * element twice.
 *
 * You can return the answer in any order.

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
