/*
 * Given an m x n 2D binary grid grid which represents a map of '1's (land) and '0's (water),
 * return the number of islands in the grid.
 *
 * An island is surrounded by water and is formed by connecting adjacent lands horizontally or
 * vertically. You may assume all four edges of the grid are all surrounded by water.
 *
 * Example 1:
 * Input: grid = [
 *    ["1","1","1","1","0"],
 *    ["1","1","0","1","0"],
 *    ["1","1","0","0","0"],
 *    ["0","0","0","0","0"]
 *  ]
 * Output: 1
 *
 * Example 2:
 * Input: grid = [
 *    ["1","1","0","0","0"],
 *    ["1","1","0","0","0"],
 *    ["0","0","1","0","0"],
 *    ["0","0","0","1","1"]
 * ]
 * Output: 3
 *
 * Constraints:
 *  m == grid.length
 *  n == grid[i].length
 *  1 <= m, n <= 300
 *  grid[i][j] is '0' or '1'.
 */

#include <array>
#include <queue>
#include <vector>

// TODO: Implement a soln w/ DFS

// version 2.01: In place grid mutation + 1D queue + BFS
//
class Solution {
public:
    static constexpr std::array<std::pair<int, int>, 4> bfs_adjacency{{
        {1, 0},   // down
        {-1, 0},  // up
        {0, 1},   // right
        {0, -1}   // left
    }};

    int numIslands(vector<vector<char>>& grid) {
        // Degenerate case: no grid
        if (grid.empty()) {
            return 0;
        }

        // row and column sizes
        auto num_rows = grid.size();
        auto num_cols = grid[0].size();

        // number of islands in the grid
        int num_of_islands{0};

        // Lambda to abstract the BFS algorithm
        auto breadth_first_search = [&](const std::pair<int, int>& start) {
            // Get BFS rolling w/ the starting element
            std::queue<int> q;
            q.push(start.first * num_cols + start.second);
            grid[start.first][start.second] = '0';  // mark as visited

            // Perform BFS
            while (!q.empty()) {
                // first step: get the first element out of the queue
                auto key = q.front();
                std::pair<int, int> next_element{key / num_cols, key % num_cols};
                q.pop();

                // Look at the 4 possible adjacent elements and decide if we need to add them
                for (const auto& adjacent_elem : bfs_adjacency) {
                    const int nr{next_element.first + adjacent_elem.first};
                    const int nc{next_element.second + adjacent_elem.second};

                    // check 1: element is in bounds of the grid
                    if (nr < 0 || nr >= num_rows || nc < 0 || nc >= num_cols) {
                        continue;
                    }
                    // check 2: element is land
                    if (grid[nr][nc] != '1') {
                        continue;
                    }

                    // all checks passed -> add element to q and visited
                    const auto neighbor = std::pair<int, int>{nr, nc};
                    grid[neighbor.first][neighbor.second] = '0';          // mark as visited
                    q.push(neighbor.first * num_cols + neighbor.second);  // grow queue
                }
            }
        };

        // Loop over the rows and cols of grid and check for connected components
        for (auto i_row = 0; i_row < num_rows; ++i_row) {
            for (auto j_col = 0; j_col < num_cols; ++j_col) {
                // Perform BFS if this is unvisited element
                if (grid[i_row][j_col] == '1') {
                    breadth_first_search({i_row, j_col});
                    num_of_islands++;
                }
            }
        }

        return num_of_islands;
    }
};

// version 1.02 - BFS w/ 1D queue + visited set
class Solution {
public:
    static constexpr std::array<std::pair<int, int>, 4> bfs_adjacency{{
        {1, 0},   // down
        {-1, 0},  // up
        {0, 1},   // right
        {0, -1}   // left
    }};

    int numIslands(vector<vector<char>>& grid) {
        // Degenerate case: no grid
        if (grid.empty()) {
            return 0;
        }

        // row and column sizes
        auto num_rows = grid.size();
        auto num_cols = grid[0].size();

        // number of islands in the grid
        int num_of_islands{0};

        // container to keep track of grid elements we have already seen
        std::set<std::pair<int, int>> visited;

        auto breadth_first_search = [&](const std::pair<int, int>& start) {
            std::queue<int> q;
            q.push(start.first * num_cols + start.second);
            visited.emplace(start);

            // Perform BFS
            while (!q.empty()) {
                // first step: get the first element out of the queue
                auto key = q.front();
                std::pair<int, int> next_element{key / num_cols, key % num_cols};
                q.pop();

                // Look at the 4 possible adjacent elements and decide if we need to add them
                for (const auto& adjacent_elem : bfs_adjacency) {
                    const int nr = next_element.first + adjacent_elem.first;
                    const int nc = next_element.second + adjacent_elem.second;

                    // check 1: the element is in bounds of the grid
                    if (nr < 0 || nr >= num_rows || nc < 0 || nc >= num_cols) {
                        continue;
                    }
                    // check 2: if element is land
                    if (grid[nr][nc] != '1') {
                        continue;
                    }
                    // check 3: if we have already seen this element
                    const auto neighbor = std::pair<int, int>{nr, nc};
                    if (visited.count(neighbor)) {
                        continue;
                    }

                    // all checks passed -> add element to q and visited
                    visited.emplace(neighbor);
                    q.push(neighbor.first * num_cols + neighbor.second);
                }
            }
        };

        // Loop over the rows and cols of grid and check for connected components
        for (auto i_row = 0; i_row < num_rows; ++i_row) {
            for (auto j_col = 0; j_col < num_cols; ++j_col) {
                if (grid[i_row][j_col] == '1' && !visited.count({i_row, j_col})) {
                    breadth_first_search({i_row, j_col});
                    num_of_islands++;
                }
            }
        }
        return num_of_islands;
    }
};

// version 1.01 - BFS w/ 2D queue + visited set
class Solution {
public:
    static constexpr std::array<std::pair<int, int>, 4> bfs_adjacency{{
        {1, 0},   // down
        {-1, 0},  // up
        {0, 1},   // right
        {0, -1}   // left
    }};

    int numIslands(vector<vector<char>>& grid) {
        // Degenerate case: no grid
        if (grid.empty()) {
            return 0;
        }

        // row and column sizes
        auto num_rows = grid.size();
        auto num_cols = grid[0].size();

        // number of islands in the grid
        int num_of_islands{0};

        // container to keep track of grid elements we have already seen
        std::set<std::pair<int, int>> visited;

        auto breadth_first_search = [&](const std::pair<int, int>& start) {
            std::queue<std::pair<int, int>> q;
            q.push({start.first, start.second});
            visited.emplace(start);

            // Perform BFS
            while (!q.empty()) {
                // first step: get the first element out of the queue
                std::pair<int, int> next_element = q.front();
                q.pop();

                // Look at the 4 possible adjacent elements and decide if we need to add them
                for (const auto& adjacent_elem : bfs_adjacency) {
                    const int nr = next_element.first + adjacent_elem.first;
                    const int nc = next_element.second + adjacent_elem.second;

                    // check 1: the element is in bounds of the grid
                    if (nr < 0 || nr >= num_rows || nc < 0 || nc >= num_cols) {
                        continue;
                    }
                    // check 2: if element is land
                    if (grid[nr][nc] != '1') {
                        continue;
                    }
                    // check 3: if we have already seen this element
                    const auto neighbor = std::pair<int, int>{nr, nc};
                    if (visited.count(neighbor)) {
                        continue;
                    }

                    // all checks passed -> add element to q and visited
                    visited.emplace(neighbor);
                    q.push({neighbor.first, neighbor.second});
                }
            }
        };

        // Loop over the rows and cols of grid and check for connected components
        for (auto i_row = 0; i_row < num_rows; ++i_row) {
            for (auto j_col = 0; j_col < num_cols; ++j_col) {
                if (grid[i_row][j_col] == '1' && !visited.count({i_row, j_col})) {
                    breadth_first_search({i_row, j_col});
                    num_of_islands++;
                }
            }
        }
        return num_of_islands;
    }
};
