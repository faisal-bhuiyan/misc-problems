// version 1.02: Some enhancements to the interview soln
//
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        // First rule of interval problems - sort it: time complexity O(nlogn)
        std::sort(intervals.begin(), intervals.end());

        // Start the results list with the first interval of sorted intervals
        std::vector<std::vector<int>> non_overlapping_intervals{{intervals[0][0], intervals[0][1]}};

        // Search for overlaps over rest of the intervals: O(n) operation
        for (auto i = 1; i < intervals.size(); ++i) {
            // detect an overlap in sorted array: start point of interval is at or before the end pt
            // of last added interval
            auto last_interval_start{non_overlapping_intervals.back()[0]};
            auto last_interval_end{non_overlapping_intervals.back()[1]};
            if (last_interval_end >= intervals[i][0]) {
                // We need to take the largest end point b/w last added and current interval
                non_overlapping_intervals.back()[1] = std::max(last_interval_end, intervals[i][1]);
                // No overlap detected -> push interval to results vector
            } else {
                non_overlapping_intervals.emplace_back(intervals[i]);
            }
        }
        return non_overlapping_intervals;
    }
};

// version 1.01: Solved during interview
//
// class Solution {
// public:
//     vector<vector<int>> merge(vector<vector<int>>& intervals) {
//         std::sort(intervals.begin(), intervals.end());
//         std::vector<std::vector<int>> non_overlapping_intervals{{intervals[0][0],
//         intervals[0][1]}}; for (auto i = 1; i < intervals.size(); ++i) {
//             if (non_overlapping_intervals.back()[0] <= intervals[i][0] &&
//             non_overlapping_intervals.back()[1] >= intervals[i][0]) {
//                 if (non_overlapping_intervals.back()[1] <= intervals[i][1]) {
//                     non_overlapping_intervals.back()[1] = intervals[i][1];
//                 }
//             } else {
//                 non_overlapping_intervals.emplace_back(intervals[i]);
//             }
//         }
//         return non_overlapping_intervals;
//     }
// };

// version 1.00: Brute force solution O(n^2)
// According to HM, the brute force soln for this problem is harder than the O(nlogn) soln
