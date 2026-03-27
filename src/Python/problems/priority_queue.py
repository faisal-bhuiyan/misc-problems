#------------------------------------------------------------------------------------------------
# Priority Queue (heap-based)
#------------------------------------------------------------------------------------------------

from __future__ import annotations

import heapq
from collections.abc import Iterable
from itertools import count
from typing import Generic, TypeVar

T = TypeVar("T")


class PriorityQueue(Generic[T]):
    """
    Min-priority queue using heapq.

    Lower numeric priority values come out first (priority=1 before priority=5).
    Ties are stable by insertion order via a monotonically increasing counter.
    """

    def __init__(self, items: Iterable[tuple[float, T]] | None = None) -> None:
        self._heap: list[tuple[float, int, T]] = []
        self._order = count()
        if items is not None:
            self.enqueue_many(items)

    def enqueue(self, item: T, priority: float = 0.0) -> None:
        """Insert an item with numeric priority (smaller value dequeues first)."""
        heapq.heappush(self._heap, (priority, next(self._order), item))

    def enqueue_many(self, items: Iterable[tuple[float, T]]) -> None:
        """Insert many (priority, item) pairs."""
        for priority, item in items:
            self.enqueue(item=item, priority=priority)

    def dequeue(self) -> T:
        """Remove and return the highest-priority item (smallest numeric priority)."""
        if self.is_empty():
            raise IndexError("dequeue from empty priority queue")
        _, _, item = heapq.heappop(self._heap)
        return item

    def peek(self) -> T:
        """Return highest-priority item without removing it."""
        if self.is_empty():
            raise IndexError("peek from empty priority queue")
        return self._heap[0][2]

    def peek_with_priority(self) -> tuple[float, T]:
        """Return (priority, item) for the next dequeue without removing it."""
        if self.is_empty():
            raise IndexError("peek from empty priority queue")
        priority, _, item = self._heap[0]
        return priority, item

    def is_empty(self) -> bool:
        return len(self._heap) == 0

    def clear(self) -> None:
        self._heap.clear()

    def __len__(self) -> int:
        return len(self._heap)

    def __bool__(self) -> bool:
        return not self.is_empty()

    def __repr__(self) -> str:
        preview = [(p, item) for p, _, item in self._heap]
        return f"PriorityQueue({preview!r})"


class SimplePriorityQueue:
    """Non-generic priority queue backed by heapq."""

    def __init__(self, items=None):
        self._heap = []
        self._order = count()
        if items is not None:
            for priority, item in items:
                self.enqueue(item, priority)

    def enqueue(self, item, priority=0):
        heapq.heappush(self._heap, (priority, next(self._order), item))

    def dequeue(self):
        if self.is_empty():
            raise IndexError("dequeue from empty priority queue")
        return heapq.heappop(self._heap)[2]

    def is_empty(self):
        return len(self._heap) == 0

    def __len__(self):
        return len(self._heap)


class MinimalPriorityQueue:
    """Minimal heapq wrapper for interview practice."""

    def __init__(self):
        self._heap = []

    def enqueue(self, item, priority=0):
        heapq.heappush(self._heap, (priority, item))

    def dequeue(self):
        if self.is_empty():
            raise IndexError("dequeue from empty priority queue")
        return heapq.heappop(self._heap)[1]

    def is_empty(self):
        return len(self._heap) == 0

    def __len__(self):
        return len(self._heap)


def _run_tests() -> None:
    print("Running PriorityQueue tests...")

    pq = PriorityQueue[str]()
    assert pq.is_empty()
    print(f"[basic] start: pq={pq}, empty={pq.is_empty()}")

    pq.enqueue("low", priority=10)
    pq.enqueue("high", priority=1)
    pq.enqueue("medium", priority=5)
    print(f"[basic] after enqueue: pq={pq}")
    assert len(pq) == 3
    assert pq.peek() == "high"
    assert pq.peek_with_priority() == (1, "high")
    print(f"[basic] peek={pq.peek()}, peek_with_priority={pq.peek_with_priority()}")

    assert pq.dequeue() == "high"
    assert pq.dequeue() == "medium"
    assert pq.dequeue() == "low"
    assert pq.is_empty()
    print(f"[basic] after three dequeues: pq={pq}")

    # Stable tie behavior
    pq.enqueue("a", priority=2)
    pq.enqueue("b", priority=2)
    pq.enqueue("c", priority=2)
    assert pq.dequeue() == "a"
    assert pq.dequeue() == "b"
    assert pq.dequeue() == "c"
    print("[tie] insertion order preserved for equal priorities")

    # Bulk enqueue + clear
    pq.enqueue_many([(3, "x"), (0, "y"), (7, "z")])
    assert list(pq.peek_with_priority()) == [0, "y"]
    pq.clear()
    assert pq.is_empty()
    print("[bulk] enqueue_many + clear passed")

    # Error paths
    try:
        pq.dequeue()
        raise AssertionError("Expected IndexError for dequeue() on empty priority queue")
    except IndexError:
        print("[errors] dequeue() on empty queue raises IndexError (expected)")
    try:
        pq.peek()
        raise AssertionError("Expected IndexError for peek() on empty priority queue")
    except IndexError:
        print("[errors] peek() on empty queue raises IndexError (expected)")

    # Simple and minimal variants smoke tests
    sq = SimplePriorityQueue([(2, "b"), (1, "a")])
    assert sq.dequeue() == "a"
    assert sq.dequeue() == "b"

    mpq = MinimalPriorityQueue()
    mpq.enqueue("urgent", 0)
    mpq.enqueue("later", 10)
    assert mpq.dequeue() == "urgent"
    assert mpq.dequeue() == "later"

    print("All PriorityQueue tests passed.")


if __name__ == "__main__":
    _run_tests()
