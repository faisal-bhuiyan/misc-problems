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
