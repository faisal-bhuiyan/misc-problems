#------------------------------------------------------------------------------------------------
# Stack (LIFO)
#------------------------------------------------------------------------------------------------

from __future__ import annotations

from collections.abc import Iterable
from typing import Generic, TypeVar

T = TypeVar("T")

class Stack(Generic[T]):
    """Simple generic stack with LIFO semantics."""

    def __init__(self, items: Iterable[T] | None = None) -> None:
        self._data: list[T] = list(items) if items is not None else []

    def push(self, item: T) -> None:
        """Push one item onto the stack."""
        self._data.append(item)

    def pop(self) -> T:
        """Pop and return the top item."""
        if self.is_empty():
            raise IndexError("pop from empty stack")
        return self._data.pop()

    def peek(self) -> T:
        """Return the top item without removing it."""
        if self.is_empty():
            raise IndexError("peek from empty stack")
        return self._data[-1]

    def top(self) -> T:
        """Alias for peek()."""
        return self.peek()

    def is_empty(self) -> bool:
        """Return True when stack has no elements."""
        return len(self._data) == 0

    def clear(self) -> None:
        """Remove all items."""
        self._data.clear()

    def __len__(self) -> int:
        return len(self._data)

    def __repr__(self) -> str:
        return f"Stack({self._data!r})"


def _run_tests() -> None:
    # Basic push/pop/peek behavior
    s = Stack[int]()
    assert s.is_empty()
    print(f"Stack after initialization: {s}")

    # Push 3 items and check the stack
    s.push(10)
    s.push(20)
    s.push(30)
    print(f"Stack after pushing 3 items: {s}")
    assert len(s) == 3

    # Check the top item and pop it
    assert s.peek() == 30
    assert s.top() == 30
    print(f"Top item: {s.peek()}")

    assert s.pop() == 30
    print(f"Stack after popping the top item: {s}")

    assert s.pop() == 20
    print(f"Stack after popping the top item: {s}")

    assert s.pop() == 10
    print(f"Stack after popping the top item: {s}")
    assert s.is_empty()

    # Iterable initialization and repr
    s2 = Stack[str](["a", "b"])
    print(f"Stack after initialization: {s2}")
    assert len(s2) == 2
    assert "Stack(" in repr(s2)
    print(f"Repr of the stack: {repr(s2)}")

    # Error paths
    try:
        s.pop()
        raise AssertionError("Expected IndexError for pop() on empty stack")
    except IndexError:
        pass
    try:
        s.peek()
        raise AssertionError("Expected IndexError for peek() on empty stack")
    except IndexError:
        pass

    # Clear
    print(f"Clearing the stack: {s2.clear()}")
    print(f"Stack after clearing: {s2}")
    assert s2.is_empty()
    print("All Stack tests passed.")


class SimpleStack:
    """Simple non-generic stack (stores any Python object)."""

    def __init__(self, items=None):
        self._data = list(items) if items is not None else []

    def push(self, item):
        self._data.append(item)

    def pop(self):
        if self.is_empty():
            raise IndexError("pop from empty stack")
        return self._data.pop()

    def peek(self):
        if self.is_empty():
            raise IndexError("peek from empty stack")
        return self._data[-1]

    def is_empty(self):
        return len(self._data) == 0

    def clear(self):
        self._data.clear()

    def __len__(self):
        return len(self._data)

    def __repr__(self):
        return f"Stack({self._data!r})"


if __name__ == "__main__":
    _run_tests()
