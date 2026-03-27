#------------------------------------------------------------------------------------------------
# Queue (FIFO)
#------------------------------------------------------------------------------------------------

from __future__ import annotations

from collections import deque
from collections.abc import Iterable
from typing import Generic, TypeVar

T = TypeVar("T")


class Queue(Generic[T]):
    """Simple generic queue with FIFO semantics."""

    def __init__(self, items: Iterable[T] | None = None) -> None:
        # Avoid `items or []` so falsey iterables (rare) are still respected.
        self._data: deque[T] = deque(items) if items is not None else deque()

    def enqueue(self, item: T) -> None:
        """Add one item to the rear of the queue."""
        self._data.append(item)

    def enqueue_many(self, items: Iterable[T]) -> None:
        """Add many items to the rear while preserving order."""
        self._data.extend(items)

    def dequeue(self) -> T:
        """Remove and return the front item."""
        if self.is_empty():
            raise IndexError("dequeue from empty queue")
        return self._data.popleft()

    def front(self) -> T:
        """Return the front item without removing it."""
        if self.is_empty():
            raise IndexError("front from empty queue")
        return self._data[0]

    def peek(self) -> T:
        """Alias for front()."""
        return self.front()

    def is_empty(self) -> bool:
        """Return True when queue has no elements."""
        return len(self._data) == 0

    def clear(self) -> None:
        """Remove all items."""
        self._data.clear()

    def __len__(self) -> int:
        return len(self._data)

    def __bool__(self) -> bool:
        return not self.is_empty()

    def __iter__(self):
        """Iterate front-to-rear."""
        return iter(self._data)

    def __repr__(self) -> str:
        return f"Queue({list(self._data)!r})"


def _run_tests() -> None:
    print("Running Queue tests...")

    # Basic enqueue/dequeue/front behavior
    q = Queue[int]()
    assert q.is_empty()
    print(f"[basic] start: q={q}, empty={q.is_empty()}")

    # Enqueue 3 items
    q.enqueue(10)
    q.enqueue(20)
    q.enqueue(30)
    print(f"[basic] after enqueue 10,20,30: q={q}")

    # Check the front item and peek it
    assert len(q) == 3
    assert q.front() == 10
    assert q.peek() == 10
    print(f"[basic] front={q.front()}, peek={q.peek()}, len={len(q)}")

    # Dequeue 3 items
    assert q.dequeue() == 10
    assert q.dequeue() == 20
    assert q.dequeue() == 30
    print(f"[basic] after three dequeues: q={q}")
    assert q.is_empty()
    assert not q

    # Bulk enqueue + bool + iterable behavior
    q.enqueue_many([1, 2, 3])
    print(f"[bulk] after enqueue_many([1,2,3]): q={q}")
    assert q
    assert list(q) == [1, 2, 3]
    assert q.dequeue() == 1
    print(f"[bulk] after one dequeue: q={q}")
    assert list(q) == [2, 3]
    q.clear()
    print(f"[bulk] after clear: q={q}, empty={q.is_empty()}")
    assert q.is_empty()

    # Iterable initialization and repr
    q2 = Queue[str](["a", "b"])
    print(f"[init] q2 from iterable: q2={q2}, len={len(q2)}")
    assert len(q2) == 2
    assert "Queue(" in repr(q2)

    try:
        q.dequeue()
        raise AssertionError("Expected IndexError for dequeue() on empty queue")
    except IndexError:
        print("[errors] dequeue() on empty queue raises IndexError (expected)")
        pass

    try:
        q.front()
        raise AssertionError("Expected IndexError for front() on empty queue")
    except IndexError:
        print("[errors] front() on empty queue raises IndexError (expected)")
        pass

    q2.clear()
    print(f"[clear] q2 after clear: q2={q2}, empty={q2.is_empty()}")
    assert q2.is_empty()

    # Falsey iterable input should still be accepted as iterable init
    q3 = Queue[int]([])
    print(f"[init] q3 from empty iterable: q3={q3}, empty={q3.is_empty()}")
    assert q3.is_empty()

    print("All Queue tests passed.")


class SimpleQueue:
    """Simple FIFO queue (non-templated)."""

    def __init__(self, items=None):
        self._data = deque(items) if items is not None else deque()

    def enqueue(self, item):
        self._data.append(item)

    def dequeue(self):
        if self.is_empty():
            raise IndexError("dequeue from empty queue")
        return self._data.popleft()

    def front(self):
        if self.is_empty():
            raise IndexError("front from empty queue")
        return self._data[0]

    def peek(self):
        return self.front()

    def is_empty(self):
        return len(self._data) == 0

    def clear(self):
        self._data.clear()

    def __len__(self):
        return len(self._data)


class MinimalQueue:
    def __init__(self):
        self._data = deque()

    def enqueue(self, item):
        self._data.append(item)

    def dequeue(self):
        if self.is_empty():
            raise IndexError("dequeue from empty queue")
        return self._data.popleft()

    def is_empty(self):
        return len(self._data) == 0

    def __len__(self):
        return len(self._data)


def _run_minimal_queue_tests() -> None:
    print("Running MinimalQueue tests...")
    q = MinimalQueue() # q = []
    q.enqueue(1) # q = [1]
    q.enqueue(2) # q = [1, 2]
    assert q.dequeue() == 1 # q = [2]
    assert len(q) == 1 # q = [2]
    assert q.is_empty() is False # q = [2]
    assert q.dequeue() == 2 # q = []
    assert len(q) == 0 # q = []
    assert q.is_empty() is True # q = []
    print("MinimalQueue tests passed.")


if __name__ == "__main__":
    _run_tests()
    _run_minimal_queue_tests()
