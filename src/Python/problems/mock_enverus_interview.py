"""
Small collection of interview-style Python problems.

Each problem has:
  - prompt-style context
  - a function solution
  - a quick demo/assert in main()
"""

from collections import deque


#------------------------------------------------------------------------------------------------
# Problem 1: Find overloaded nodes
#------------------------------------------------------------------------------------------------
#
# Prompt: Write a function that returns all node IDs whose load exceeds a threshold.

def find_overloaded_nodes(nodes: list[tuple[str, int]], threshold: float) -> list[str]:
    return [node_id for node_id, load in nodes if load > threshold]


#------------------------------------------------------------------------------------------------
# Problem 2: Grid connectivity
#------------------------------------------------------------------------------------------------
#
# Prompt: Determine whether all substations are connected.

def is_grid_connected(num_nodes: int, edges: list[tuple[int, int]]) -> bool:
    """
    Determine whether the grid is fully connected.

    Parameters
    ----------
    num_nodes : int
        Number of nodes (0 .. n-1).
    edges : list[tuple[int, int]]
        Transmission lines between nodes.

    Returns
    -------
    bool
        True if every node is reachable from every other node.
    """

    # corner case: just one node
    if num_nodes <= 1:
        return True

    # First let's build the adjacency list for the nodes
    adjacency_list = [[] for _ in range(num_nodes)]
    for n0, n1 in edges:
        # Add nodes to the adjacency list if they are within the range of the graph
        if 0 <= n0 < num_nodes and 0 <= n1 < num_nodes:
            adjacency_list[n0].append(n1)
            adjacency_list[n1].append(n0) # undirected graph
    # Now we can start at node 0 and check if we can visit all the nodes from it
    # by following the adjacency list

    # use a set to keep track of the nodes we visited, starting w/ 0
    visited_nodes: set[int] = {0}
    # use a stack (LIFO) to to keep track of the depth-first search
    stack = [0]
    while stack:
        # Take the last element out
        n0 = stack.pop()
        for node in adjacency_list[n0]:
            if node not in visited_nodes:
                visited_nodes.add(node)
                stack.append(node)

    # Grid is fully connected if the visited_node contains all the nodes in the graph
    return len(visited_nodes) == num_nodes

#------------------------------------------------------------------------------------------------
# Problem 3: Cascading grid failure simulation
#------------------------------------------------------------------------------------------------
#
# Prompt: Simulate cascading failures with load redistribution.

def simulate_cascade(
    n: int,
    edges: list[tuple[int, int]],
    loads: list[float],
    capacity: list[float],
) -> set[int]:
    """
    Simulate cascading grid failures.

    Parameters
    ----------
    n : int
        Number of nodes (0 .. n-1).
    edges : list[tuple[int, int]]
        Transmission lines between nodes.
    loads : list[float]
        Current load at each node.
    capacity : list[float]
        Maximum capacity for each node.

    Returns
    -------
    set[int]
        The set of nodes that failed during the cascade.
    """

    # --------------------------------------------------
    # Step 1: Build adjacency list
    # --------------------------------------------------
    graph = [[] for _ in range(n)]
    for a, b in edges:
        graph[a].append(b)
        graph[b].append(a)

    # --------------------------------------------------
    # Step 2: Track failed nodes
    # --------------------------------------------------
    failed: set[int] = set()

    # Queue used for BFS-style propagation
    queue: deque[int] = deque()

    # --------------------------------------------------
    # Step 3: Detect initial failures
    # --------------------------------------------------
    if len(loads) != n or len(capacity) != n:
        raise ValueError("loads and capacity must each have length n")

    # Work on a mutable copy so callers keep their original list.
    cur_loads = list(loads)

    for node in range(n):
        if cur_loads[node] > capacity[node]:
            failed.add(node)
            queue.append(node)

    # --------------------------------------------------
    # Step 4: Process cascading failures
    # --------------------------------------------------
    while queue:
        node = queue.popleft()

        # Redistribute failed-node load evenly to *active* neighbors only.
        active_neighbors = [nei for nei in graph[node] if nei not in failed]
        if not active_neighbors:
            # No active neighbor can absorb load; load effectively shed.
            cur_loads[node] = 0.0
            continue

        share = cur_loads[node] / len(active_neighbors)
        cur_loads[node] = 0.0
        for nei in active_neighbors:
            cur_loads[nei] += share
            if cur_loads[nei] > capacity[nei] and nei not in failed:
                failed.add(nei)
                queue.append(nei)

    return failed


#------------------------------------------------------------------------------------------------
# Test cases for the problems
#------------------------------------------------------------------------------------------------

def demo_problem_1() -> None:
    """
    Run Problem 1 input/output check and print result.
    """
    nodes = [
        ("N1", 120),
        ("N2", 80),
        ("N3", 200),
        ("N4", 95),
        ("N5", 300),
    ]
    threshold = 100
    expected_overloaded = ["N1", "N3", "N5"]
    computed_overloaded = find_overloaded_nodes(nodes, threshold)
    assert computed_overloaded == expected_overloaded
    print(f"Problem 1 - overloaded node IDs: {computed_overloaded}")


def demo_problem_2() -> None:
    """
    Run Problem 2 input/output check and print result.
    """
    print("\nProblem 2 - Grid Connectivity")

    # Test Case 1
    n = 5
    edges = [(0, 1), (1, 2), (2, 3), (3, 4)]
    expected = True
    result = is_grid_connected(n, edges)
    assert result == expected
    print(f"  Case 1 expected={expected}, result={result}")

    # Test Case 2
    n = 5
    edges = [(0, 1), (1, 2), (3, 4)]
    expected = False
    result = is_grid_connected(n, edges)
    assert result == expected
    print(f"  Case 2 expected={expected}, result={result}")


def demo_problem_3() -> None:
    """
    Run Problem 3 input/output check and print result.
    """
    print("\nProblem 3 - Cascading Grid Failure Simulation")

    n = 4
    edges = [(0, 1), (1, 2), (1, 3)]
    loads = [40, 120, 30, 20]
    capacity = [100, 100, 100, 100]

    print(f"  Nodes: {n}")
    print(f"  Edges: {edges}")
    print(f"  Loads: {loads}")
    print(f"  Capacity: {capacity}")

    result = simulate_cascade(n, edges=edges, loads=loads, capacity=capacity)
    expected = {1}
    assert result == expected
    print(f"  Expected failed nodes: {expected}")
    print(f"  Failed nodes: {result}")


#------------------------------------------------------------------------------------------------
# Main function
#------------------------------------------------------------------------------------------------

def main() -> None:
    """Run all problem demos."""
    print("\n==============================")
    print("Mock Interview Problems")
    print("==============================")
    demo_problem_1()
    demo_problem_2()
    demo_problem_3()


if __name__ == "__main__":
    main()

