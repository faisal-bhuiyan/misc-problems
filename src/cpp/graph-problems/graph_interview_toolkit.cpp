#include <cstddef>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Build adjacency list for an undirected graph
 *
 * Each edge (u, v) is recorded in both directions: u -> v and v -> u.
 * Nodes are labeled 0 .. num_nodes-1.
 *
 * Time:  O(V + E)
 * Space: O(V + E)
 *
 * @param num_nodes  Number of nodes in the graph.
 * @param edges      Pairs (u, v) representing undirected edges.
 * @return           adjacency list where adj_list[u] lists all neighbors of u.
 * @throws           std::invalid_argument if any endpoint is outside [0, num_nodes-1].
 */
std::vector<std::vector<int>> build_undirected_adjacency_list(
    int num_nodes, const std::vector<std::pair<int, int>>& edges
) {
    std::vector<std::vector<int>> adj_list(static_cast<std::size_t>(num_nodes));

    // Build adjacency list
    for (const auto& edge : edges) {
        const int source_node{edge.first};
        const int destination_node{edge.second};

        // Validate endpoints are within valid range
        if (source_node < 0 || source_node >= num_nodes || destination_node < 0 ||
            destination_node >= num_nodes) {
            throw std::invalid_argument(
                "edge (" + std::to_string(source_node) + ", " + std::to_string(destination_node) +
                ") has endpoint outside [0, " + std::to_string(num_nodes - 1) + "]"
            );
        }

        // Add edge to adjacency list in both directions
        adj_list[static_cast<std::size_t>(source_node)].push_back(destination_node);
        adj_list[static_cast<std::size_t>(destination_node)].push_back(
            source_node  // undirected: add both directions
        );
    }

    return adj_list;
}

/**
 * @brief Build adjacency list for a directed graph
 *
 * Each edge (u, v) is recorded only as u -> v; there is no reverse edge.
 * Nodes are labeled 0 .. num_nodes-1.
 *
 * Time:  O(V + E)
 * Space: O(V + E)
 *
 * @param num_nodes  Number of nodes in the graph.
 * @param edges      Pairs (u, v) representing directed edges u -> v.
 * @return           adjacency list where adj_list[u] lists all out-neighbors of u.
 * @throws           std::invalid_argument if any endpoint is outside [0, num_nodes-1].
 */
std::vector<std::vector<int>> build_directed_adjacency_list(
    int num_nodes, const std::vector<std::pair<int, int>>& edges
) {
    std::vector<std::vector<int>> adj_list(static_cast<std::size_t>(num_nodes));

    // Build adjacency list
    for (const auto& edge : edges) {
        const int source_node{edge.first};
        const int destination_node{edge.second};

        // Validate endpoints are within valid range
        if (source_node < 0 || source_node >= num_nodes || destination_node < 0 ||
            destination_node >= num_nodes) {
            throw std::invalid_argument(
                "edge (" + std::to_string(source_node) + ", " + std::to_string(destination_node) +
                ") has endpoint outside [0, " + std::to_string(num_nodes - 1) + "]"
            );
        }

        // Add edge to adjacency list in one direction
        adj_list[static_cast<std::size_t>(source_node)].push_back(
            destination_node  // directed graph -> add edge in one direction only
        );
    }

    return adj_list;
}

/**
 * @brief Determine whether undirected graph is fully connected using BFS
 *
 * BFS explores layer by layer (FIFO queue). Starting from node 0, it visits
 * all reachable nodes. If every node is reached, the graph is connected.
 *
 * BFS vs DFS for connectivity: both are O(V+E); BFS naturally finds the
 * shortest hop-count path to each node as a side effect.
 *
 * Time:  O(V + E)
 * Space: O(V)  — visited array + queue
 *
 * @param num_nodes  Number of nodes (0 .. num_nodes-1).
 * @param edges      Undirected edges.
 * @return           true if every node is reachable from node 0.
 */
bool bfs_connected_graph(int num_nodes, const std::vector<std::pair<int, int>>& edges) {
    // Degenerate case: trivially connected
    if (num_nodes <= 1) {
        return true;
    }

    // Build adjacency list
    const std::vector<std::vector<int>> adj_list = build_undirected_adjacency_list(num_nodes, edges);

    // Use char instead of bool to avoid std::vector<bool> bit-packing issues
    // 0 -> not visited, 1 -> visited
    std::vector<char> visited(static_cast<std::size_t>(num_nodes), 0);
    std::deque<int> queue;

    // Seed BFS from node 0
    visited[0] = 1;      // mark node 0 as visited
    queue.push_back(0);  // add node 0 to queue

    while (!queue.empty()) {
        // Get the next node to visit from the FIFO queue
        const int current_node{queue.front()};
        queue.pop_front();

        // Visit all neighbors of the current node
        for (int neighbor : adj_list[static_cast<std::size_t>(current_node)]) {
            // If the neighbor has not been visited, mark it as visited and add it to the queue
            if (!visited[static_cast<std::size_t>(neighbor)]) {
                visited[static_cast<std::size_t>(neighbor)] = 1;  // mark neighbor as visited
                queue.push_back(neighbor);                        // add neighbor to queue
            }
        }
    }

    // Connected iff every node was reached
    for (char was_visited : visited) {
        // If any node was not visited, the graph is not connected
        if (!was_visited) {
            return false;
        }
    }
    // If all nodes were visited, the graph is connected
    return true;
}

/**
 * @brief Determine whether an undirected graph is fully connected using DFS.
 *
 * DFS explores as deep as possible before backtracking (LIFO stack). Starting
 * from node 0, it visits all reachable nodes. If every node is reached, the
 * graph is connected.
 *
 * This is an iterative DFS (explicit stack) to avoid call-stack overflow on
 * large inputs. Note: iterative DFS visits neighbors in reverse order compared
 * to recursive DFS, but the connectivity result is identical.
 *
 * Time:  O(V + E)
 * Space: O(V)  — visited array + stack
 *
 * @param num_nodes  Number of nodes (0 .. num_nodes-1).
 * @param edges      Undirected edges.
 * @return           true if every node is reachable from node 0.
 */
bool dfs_connected_graph(int num_nodes, const std::vector<std::pair<int, int>>& edges) {
    if (num_nodes <= 1) {
        return true;  // trivially connected
    }

    const std::vector<std::vector<int>> adj_list = build_undirected_adjacency_list(num_nodes, edges);

    std::vector<char> visited(static_cast<std::size_t>(num_nodes), 0);
    std::vector<int> stack;

    // Seed DFS from node 0
    visited[0] = 1;
    stack.push_back(0);

    while (!stack.empty()) {
        const int current_node{stack.back()};
        stack.pop_back();

        for (int neighbor : adj_list[static_cast<std::size_t>(current_node)]) {
            if (!visited[static_cast<std::size_t>(neighbor)]) {
                visited[static_cast<std::size_t>(neighbor)] = 1;
                stack.push_back(neighbor);
            }
        }
    }

    // Connected iff every node was reached
    for (char was_visited : visited) {
        if (!was_visited) {
            return false;
        }
    }
    return true;
}

namespace {

void run_smoke_tests() {
    // A simple chain 0-1-2-3-4: fully connected
    const std::vector<std::pair<int, int>> chain = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    if (!bfs_connected_graph(5, chain) || !dfs_connected_graph(5, chain)) {
        throw std::runtime_error("smoke: chain should be connected");
    }

    // Two disjoint components {0,1,2} and {3,4}: not fully connected
    const std::vector<std::pair<int, int>> two_components = {{0, 1}, {1, 2}, {3, 4}};
    if (bfs_connected_graph(5, two_components) || dfs_connected_graph(5, two_components)) {
        throw std::runtime_error("smoke: two_components should not be connected");
    }

    std::cout << "[ok] graph toolkit smoke tests passed.\n";
}

}  // namespace

int main() {
    try {
        run_smoke_tests();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
