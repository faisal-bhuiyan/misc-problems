/*
 * An axis-aligned rectangle is represented as a list [x1, y1, x2, y2], where (x1, y1) is the
 coordinate of its bottom-left corner, and (x2, y2) is the coordinate of its top-right corner. Its
 top and bottom edges are parallel to the X-axis, and its left and right edges are parallel to the
 Y-axis.

 * Two rectangles overlap if the area of their intersection is positive. To be clear, two rectangles
 that only touch at the corner or edges do not overlap.
 *
 * Given two axis-aligned rectangles rec1 and rec2, return true if they overlap, otherwise return
 false.

 * Example 1:
 * Input: rec1 = [0,0,2,2], rec2 = [1,1,3,3]
 * Output: true
 *
 * Example 2:
 * Input: rec1 = [0,0,1,1], rec2 = [1,0,2,1]
 * Output: false
 *
 * Example 3:
 * Input: rec1 = [0,0,1,1], rec2 = [2,2,3,3]
 * Output: false
 */

#include <algorithm>
#include <vector>

// version 1.01: axis aligned bounding box intersection detection
// This is analogous to detecting intersection in 1D intervals,
// however more complex due to increased number of ways an intersection
// can happen. Following is the logic:
// - sort two rectangles by their starting point x coord -> smallest is rec1, >= is rec2
// - intersection can happen only if:
// - rect2.start must be < than rec1.end (strict inequality)
// - rec2.start.y must be >= rec1.start.y (equality is fine here)
//
class Solution {
public:
    bool isRectangleOverlap(vector<int>& rec1, vector<int>& rec2) {
        // sort rectangles by starting x coord
        if (rec1[0] > rec2[0]) {
            std::swap(rec1, rec2);
        }
        // check for intersection
        if (rec2[0] < rec1[2] && rec2[1] < rec1[3] && rec2[3] >= rec1[1]) {
            return true;
        }
        return false;
    }
};
