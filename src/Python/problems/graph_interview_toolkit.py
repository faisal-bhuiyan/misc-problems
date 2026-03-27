"""
Graph interview toolkit (reusable template).

Quick mapping (problem -> data structure):
- Connectivity / components -> adjacency + visited + DFS/BFS
- Unweighted shortest path -> queue + distance
- Weighted shortest path -> priority queue + distance
- Cycle in undirected graph -> DFS parent tracking or Union-Find
- Topological ordering -> in-degree + queue or DFS stack
"""

from __future__ import annotations

from collections import deque
import heapq


def build_undirected_adj_list(n: int, edges: list[tuple[int, int]]) -> list[list[int]]:
    """Build an undirected adjacency list for nodes 0..n-1."""
    adj = [[] for _ in range(n)]
    for u, v in edges:
        if not (0 <= u < n and 0 <= v < n):
            raise ValueError(f"edge ({u}, {v}) has endpoint outside [0, {n - 1}]")
        adj[u].append(v)
        adj[v].append(u)
    return adj


def build_directed_adj_list(n: int, edges: list[tuple[int, int]]) -> list[list[int]]:
    """Build a directed adjacency list for nodes 0..n-1."""
    adj = [[] for _ in range(n)]
    for u, v in edges:
        if not (0 <= u < n and 0 <= v < n):
            raise ValueError(f"edge ({u}, {v}) has endpoint outside [0, {n - 1}]")
        adj[u].append(v)
    return adj


def bfs_connected(n: int, edges: list[tuple[int, int]]) -> bool:
    """Return True if an undirected graph is fully connected."""
    if n <= 1:
        return True
    adj = build_undirected_adj_list(n, edges)
    visited = {0}
    q = deque([0])
    while q:
        u = q.popleft()
        for v in adj[u]:
            if v not in visited:
                visited.add(v)
                q.append(v)
    return len(visited) == n


def dfs_connected(n: int, edges: list[tuple[int, int]]) -> bool:
    """Return True if an undirected graph is fully connected."""
    if n <= 1:
        return True
    adj = build_undirected_adj_list(n, edges)
    visited = {0}
    stack = [0]
    while stack:
        u = stack.pop()
        for v in adj[u]:
            if v not in visited:
                visited.add(v)
                stack.append(v)
    return len(visited) == n


def shortest_path_unweighted(
    n: int, edges: list[tuple[int, int]], src: int
) -> list[int | None]:
    """Single-source shortest path in an unweighted undirected graph via BFS."""
    if not (0 <= src < n):
        raise ValueError("src out of range")
    adj = build_undirected_adj_list(n, edges)
    dist: list[int | None] = [None] * n
    dist[src] = 0
    q = deque([src])
    while q:
        u = q.popleft()
        assert dist[u] is not None
        for v in adj[u]:
            if dist[v] is None:
                dist[v] = dist[u] + 1
                q.append(v)
    return dist


def dijkstra(
    n: int, weighted_edges: list[tuple[int, int, float]], src: int
) -> list[float]:
    """
    Single-source shortest path for non-negative edge weights (undirected).

    weighted_edges format: (u, v, w).
    """
    if not (0 <= src < n):
        raise ValueError("src out of range")
    adj: list[list[tuple[int, float]]] = [[] for _ in range(n)]
    for u, v, w in weighted_edges:
        if w < 0:
            raise ValueError("Dijkstra requires non-negative weights")
        if not (0 <= u < n and 0 <= v < n):
            raise ValueError(f"edge ({u}, {v}) has endpoint outside [0, {n - 1}]")
        adj[u].append((v, w))
        adj[v].append((u, w))

    dist = [float("inf")] * n
    dist[src] = 0.0
    pq: list[tuple[float, int]] = [(0.0, src)]  # (distance, node)
    while pq:
        cur_d, u = heapq.heappop(pq)
        if cur_d > dist[u]:
            continue
        for v, w in adj[u]:
            cand = cur_d + w
            if cand < dist[v]:
                dist[v] = cand
                heapq.heappush(pq, (cand, v))
    return dist


def has_cycle_undirected_dfs(n: int, edges: list[tuple[int, int]]) -> bool:
    """Detect cycle in an undirected graph via DFS parent tracking."""
    adj = build_undirected_adj_list(n, edges)
    visited = [False] * n

    for start in range(n):
        if visited[start]:
            continue
        stack: list[tuple[int, int]] = [(start, -1)]  # (node, parent)
        visited[start] = True
        while stack:
            u, parent = stack.pop()
            for v in adj[u]:
                if not visited[v]:
                    visited[v] = True
                    stack.append((v, u))
                elif v != parent:
                    return True
    return False


class UnionFind:
    """Disjoint Set Union (Union-Find) with path compression + union by rank."""

    def __init__(self, n: int) -> None:
        self.parent = list(range(n))
        self.rank = [0] * n

    def find(self, x: int) -> int:
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, a: int, b: int) -> bool:
        """Union sets of a and b. Returns False when already in same set."""
        ra, rb = self.find(a), self.find(b)
        if ra == rb:
            return False
        if self.rank[ra] < self.rank[rb]:
            self.parent[ra] = rb
        elif self.rank[ra] > self.rank[rb]:
            self.parent[rb] = ra
        else:
            self.parent[rb] = ra
            self.rank[ra] += 1
        return True


def has_cycle_undirected_union_find(n: int, edges: list[tuple[int, int]]) -> bool:
    """Detect cycle in an undirected graph via Union-Find."""
    uf = UnionFind(n)
    for u, v in edges:
        if not uf.union(u, v):
            return True
    return False


def topo_sort_kahn(n: int, edges: list[tuple[int, int]]) -> list[int]:
    """Topological ordering for a directed graph (raises on cycle)."""
    adj = build_directed_adj_list(n, edges)
    indeg = [0] * n
    for u in range(n):
        for v in adj[u]:
            indeg[v] += 1

    q = deque([i for i in range(n) if indeg[i] == 0])
    order: list[int] = []
    while q:
        u = q.popleft()
        order.append(u)
        for v in adj[u]:
            indeg[v] -= 1
            if indeg[v] == 0:
                q.append(v)
    if len(order) != n:
        raise ValueError("Graph has a cycle; topological ordering does not exist")
    return order


def topo_sort_dfs(n: int, edges: list[tuple[int, int]]) -> list[int]:
    """Topological ordering via DFS with recursion-state cycle detection."""
    adj = build_directed_adj_list(n, edges)
    state = [0] * n  # 0=unseen, 1=visiting, 2=done
    out: list[int] = []

    def dfs(u: int) -> None:
        if state[u] == 1:
            raise ValueError("Graph has a cycle; topological ordering does not exist")
        if state[u] == 2:
            return
        state[u] = 1
        for v in adj[u]:
            dfs(v)
        state[u] = 2
        out.append(u)

    for i in range(n):
        if state[i] == 0:
            dfs(i)
    out.reverse()
    return out


def _run_smoke_tests() -> None:
    print("Running graph interview toolkit smoke tests...")

    # Connectivity / components
    assert bfs_connected(5, [(0, 1), (1, 2), (2, 3), (3, 4)]) is True
    assert dfs_connected(5, [(0, 1), (1, 2), (3, 4)]) is False
    print("[ok] connectivity (BFS/DFS)")

    # Unweighted shortest path
    d = shortest_path_unweighted(5, [(0, 1), (1, 2), (2, 3), (3, 4)], src=0)
    assert d == [0, 1, 2, 3, 4]
    print("[ok] unweighted shortest path (BFS)")

    # Weighted shortest path
    w = [(0, 1, 2.0), (1, 2, 1.0), (0, 2, 10.0), (2, 3, 0.5)]
    dd = dijkstra(4, w, src=0)
    assert dd == [0.0, 2.0, 3.0, 3.5]
    print("[ok] weighted shortest path (Dijkstra)")

    # Cycle detection (undirected)
    cyc_edges = [(0, 1), (1, 2), (2, 0)]
    acyc_edges = [(0, 1), (1, 2), (2, 3)]
    assert has_cycle_undirected_dfs(4, cyc_edges) is True
    assert has_cycle_undirected_dfs(4, acyc_edges) is False
    assert has_cycle_undirected_union_find(4, cyc_edges) is True
    assert has_cycle_undirected_union_find(4, acyc_edges) is False
    print("[ok] cycle detection (DFS parent / Union-Find)")

    # Topological ordering (DAG)
    dag_edges = [(0, 1), (0, 2), (1, 3), (2, 3)]
    kahn = topo_sort_kahn(4, dag_edges)
    dfs_order = topo_sort_dfs(4, dag_edges)
    assert kahn[0] == 0 and kahn[-1] == 3
    assert dfs_order[0] == 0 and dfs_order[-1] == 3
    print("[ok] topological sort (Kahn / DFS)")

    print("All graph toolkit smoke tests passed.")


if __name__ == "__main__":
    _run_smoke_tests()
