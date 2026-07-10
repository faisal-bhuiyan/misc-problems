/*
 * Given an array of intervals where intervals[i] = [start_i, end_i], merge all overlapping
 * intervals, and return an array of the non-overlapping intervals that cover all the intervals
 * in the input.
 *
 * Example 1:
 * Input: intervals = [[1,3],[2,6],[8,10],[15,18]]
 * Output: [[1,6],[8,10],[15,18]]
 * Explanation: Since intervals [1,3] and [2,6] overlap, merge them into [1,6].
 *
 * Example 2:
 * Input: intervals = [[1,4],[4,5]]
 * Output: [[1,5]]
 * Explanation: Intervals [1,4] and [4,5] are considered overlapping.
 *
 * Example 3:
 * Input: intervals = [[4,7],[1,4]]
 * Output: [[1,7]]
 * Explanation: Intervals [1,4] and [4,7] are considered overlapping.
 */

#include <algorithm>
#include <vector>

// version 1.02: Some enhancements to the interview soln
//
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        // First rule of interval problems - sort it: time complexity O(n * logn)
        std::sort(intervals.begin(), intervals.end());

        // Start the results list with the first interval of sorted intervals
        std::vector<std::vector<int>> non_overlapping_intervals{{intervals[0][0], intervals[0][1]}};

        // Search for overlaps over rest of the intervals: O(n) operation
        for (auto i = 1; i < intervals.size(); ++i) {
            // Detect an overlap in sorted array ->
            // start point of interval is at or before the end pt of last added interval
            auto last_interval_end{non_overlapping_intervals.back()[1]};
            // Case 1: Overlap detected
            if (last_interval_end >= intervals[i][0]) {
                // We need to take the largest end point b/w last added and current interval
                non_overlapping_intervals.back()[1] = std::max(last_interval_end, intervals[i][1]);
                // Case 2: No overlap detected -> push interval to results vector
            } else {
                non_overlapping_intervals.emplace_back(intervals[i]);
            }
        }
        return non_overlapping_intervals;
    }
};

// version 1.01: Solved during interview
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        std::sort(intervals.begin(), intervals.end());
        std::vector<std::vector<int>> non_overlapping_intervals{{intervals[0][0], intervals[0][1]}};
        for (auto i = 1; i < intervals.size(); ++i) {
            if (non_overlapping_intervals.back()[0] <= intervals[i][0] &&
                non_overlapping_intervals.back()[1] >= intervals[i][0]) {
                if (non_overlapping_intervals.back()[1] <= intervals[i][1]) {
                    non_overlapping_intervals.back()[1] = intervals[i][1];
                }
            } else {
                non_overlapping_intervals.emplace_back(intervals[i]);
            }
        }
        return non_overlapping_intervals;
    }
};

// version 1.00: Brute force solution O(n^2)
// According to HM, the brute force soln for this problem is harder than the O(nlogn) soln
