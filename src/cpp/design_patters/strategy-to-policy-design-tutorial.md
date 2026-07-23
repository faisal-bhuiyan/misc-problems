# From Strategy to Policy-Based Design in C++

## Part 1 — The problem addressed by Strategy

The runtime Strategy pattern described by the Gang of Four and compile-time
policy-based design address the same design question:

> A class performs an operation whose implementation may vary, while the remainder
> of the class remains stable. How can that variation be introduced without
> duplicating or repeatedly modifying the class?

For example, a point-to-point distance operation may use either Euclidean or
Manhattan distance. Similarly, a geometric predicate may use either a fixed
tolerance or one derived from the scale of the model. In each case, the operation
has a stable interface, but its implementation varies.

Without an explicit variation point, code commonly develops one of two problems:
- conditionals such as `if (mode == kEuclidean) ... else if (...)` become
  duplicated across call sites; or
- a class hard-codes one implementation and must be modified or copied whenever
  another implementation is required.

Both designs separate the varying operation from the code that uses it. The
surrounding class depends on a defined interface rather than on a particular
implementation.

---

## Part 2 — Classic Strategy: runtime polymorphism

The classic Strategy pattern defines an abstract interface for the varying
operation and supplies a concrete implementation at runtime.

```cpp
#include <memory>
#include <cmath>
#include <utility>
#include <vector>

struct Point { double x{}, y{}; };

// Interface for the operation that varies.
class DistanceStrategy {
public:
    virtual ~DistanceStrategy() = default;
    virtual double Distance(const Point& a, const Point& b) const = 0;
};

class EuclideanDistance : public DistanceStrategy {
public:
    double Distance(const Point& a, const Point& b) const override {
        return std::hypot(a.x - b.x, a.y - b.y);
    }
};

class ManhattanDistance : public DistanceStrategy {
public:
    double Distance(const Point& a, const Point& b) const override {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
};

// Context: uses a strategy without depending on a concrete implementation.
class NearestNeighborFinder {
public:
    explicit NearestNeighborFinder(std::unique_ptr<DistanceStrategy> strategy)
        : strategy_{std::move(strategy)} {}

    void SetStrategy(std::unique_ptr<DistanceStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    // Precondition: candidates is not empty.
    Point FindNearest(const Point& query, const std::vector<Point>& candidates) const {
        Point best{candidates.front()};
        double best_dist{strategy_->Distance(query, best)};
        for (const auto& candidate : candidates) {
            const double dist{strategy_->Distance(query, candidate)};
            if (dist < best_dist) {
                best = candidate;
                best_dist = dist;
            }
        }
        return best;
    }

private:
    std::unique_ptr<DistanceStrategy> strategy_;
};
```

`NearestNeighborFinder` does not refer to either concrete distance calculation.
The program can select a strategy from user input or configuration and replace it
through `SetStrategy` while running. A new distance metric can be introduced as
another implementation of `DistanceStrategy` without changing
`NearestNeighborFinder`. This is the Open/Closed Principle in its conventional
object-oriented form.

**Costs and limitations:**
- `strategy_->Distance(...)` normally uses virtual dispatch. Unless the compiler can
  devirtualize the call, this adds an indirect call and may prevent inlining.
- In this implementation, constructing a strategy with `std::make_unique` allocates
  the strategy object dynamically. The `std::unique_ptr` also introduces pointer
  indirection. Runtime Strategy does not inherently require dynamic allocation, but
  owning a polymorphic object this way is common.
- These costs are usually insignificant for operations performed infrequently. In a
  hot numerical loop, however, inhibited inlining and optimization may be measurable
  and should be evaluated with profiling and benchmarks.

Runtime Strategy therefore exchanges some optimization opportunities for runtime
extensibility. That exchange is appropriate when the selected behavior is genuinely
a runtime concern. It may be unnecessary when the behavior is fixed for the lifetime
of a compiled algorithm.

---

## Part 3 — Determine when the variation is selected

The central design question is whether the implementation must be selected at
runtime. A user-facing application might allow the distance metric to be changed
through configuration or an interface. By contrast, a specific numerical algorithm
may define its metric as part of its type and never change it during execution.

When selection is known at compile time, policy-based design represents the same
variation with a template parameter rather than a virtual base class. This permits
static type checking and gives the optimizer visibility into the concrete
implementation.

---

## Part 4 — Policy-based design: compile-time selection

```cpp
#include <cmath>
#include <concepts>
#include <vector>

struct Point { double x{}, y{}; };

// A policy is any type that satisfies this interface; inheritance is not required.
template <typename P>
concept DistancePolicy = requires(P policy, Point a, Point b) {
    { policy.Distance(a, b) } -> std::convertible_to<double>;
};

struct EuclideanPolicy {
    double Distance(const Point& a, const Point& b) const {
        return std::hypot(a.x - b.x, a.y - b.y);
    }
};

struct ManhattanPolicy {
    double Distance(const Point& a, const Point& b) const {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
};

// The policy is selected through a template parameter.
template <DistancePolicy Policy>
class NearestNeighborFinder {
public:
    // Precondition: candidates is not empty.
    Point FindNearest(const Point& query, const std::vector<Point>& candidates) const {
        Point best{candidates.front()};
        double best_dist{policy_.Distance(query, best)};
        for (const auto& candidate : candidates) {
            const double dist{policy_.Distance(query, candidate)};
            if (dist < best_dist) {
                best = candidate;
                best_dist = dist;
            }
        }
        return best;
    }

private:
    [[no_unique_address]] Policy policy_{};
};

// The policy is selected at the call site and forms part of the type.
NearestNeighborFinder<EuclideanPolicy> euclidean_finder;
NearestNeighborFinder<ManhattanPolicy> manhattan_finder;
```

**Mechanical differences:**
- `policy_.Distance(...)` invokes a concrete type known at compile time. The compiler
  can usually inline the function and optimize across the call.
- The example policies contain no state, require no virtual table, and require no
  dynamic allocation. `[[no_unique_address]]` allows an empty policy member to
  occupy no additional storage when the object layout permits it.
- The `DistancePolicy` concept specifies the required expression and return type.
  The compiler checks this structural interface when the template is instantiated;
  no inheritance relationship is required.

**Trade-off:** each policy produces a distinct `NearestNeighborFinder` type. This
form cannot replace the policy of an existing object with an unrelated policy type
at runtime. If runtime selection is required, use runtime polymorphism or, for a
closed set of alternatives, a discriminated union such as `std::variant`.

Policy-based design preserves the separation between an operation and its
implementation, but resolves that relationship through templates instead of virtual
dispatch. The appropriate mechanism depends on when the implementation must be
selected.

---

## Part 5 — Multiple independent policies

Policy-based design is particularly useful when a class has several independent
variation points. Each concern can be represented by a separate template parameter,
allowing implementations to be combined without defining a class for every
combination.

Consider a `Grid` whose cell storage and out-of-bounds behavior vary independently:

```cpp
// Storage policies.
struct DenseStorage {
    // Store every cell explicitly, for example in std::vector<Cell>.
};
struct SparseStorage {
    // Store only non-empty cells, for example in
    // std::unordered_map<Index, Cell>.
};

// Bounds policies.
struct ClampBounds {
    // Clamp an out-of-range index to the nearest valid index.
};
struct WrapBounds {
    // Wrap an out-of-range index to implement periodic boundaries.
};

// The host accepts each concern as an independent policy.
template <typename StoragePolicy, typename BoundsPolicy>
class Grid : private StoragePolicy, private BoundsPolicy {
    // Delegate storage operations to StoragePolicy and index normalization
    // to BoundsPolicy.
};

// Each combination is available without a separate named subclass.
Grid<DenseStorage, ClampBounds>  grid_a;
Grid<SparseStorage, WrapBounds>  grid_b;
Grid<DenseStorage, WrapBounds>   grid_c;
Grid<SparseStorage, ClampBounds> grid_d;
```

Encoding both concerns in one class hierarchy would tend to produce classes such as
`DenseClampGrid` and `SparseWrapGrid`. Independent policies instead represent the
cross-product directly. Runtime composition with two Strategy objects can provide
the same separation when runtime selection is required; policy parameters provide
the compile-time form and allow calls to be inlined.

The example uses private inheritance so that empty policy types may benefit from the
Empty Base Optimization. In C++20, composition with
`[[no_unique_address]] Policy policy_;` is often a clearer alternative. Stateful
policies still require storage in either representation.

---

## Part 6 — Example: configurable numerical tolerance

A single global tolerance is often unsuitable for geometry represented at widely
different coordinate scales. The method used to calculate tolerance is itself a
variation point and can therefore be expressed as a policy.

The following examples assume that `ScalarType` is an existing floating-point
concept.

**Step 1 — define the required interface:**

```cpp
#include <concepts>

template <typename P, typename T>
concept TolerancePolicy = requires(const P& policy) {
    { policy.Tolerance() } -> std::convertible_to<T>;
};
```

**Step 2 — provide concrete policies:**

```cpp
#include <algorithm>
#include <cmath>

// A fixed absolute tolerance.
template <ScalarType T>
struct FixedTolerance {
    T value{static_cast<T>(1e-9)};
    T Tolerance() const { return value; }
};

// A scale-aware tolerance with an absolute lower bound.
template <ScalarType T>
struct ScaledTolerance {
    T bounding_box_diagonal{static_cast<T>(1.0)};
    T relative_epsilon{static_cast<T>(1e-9)};
    T absolute_floor{static_cast<T>(1e-12)};

    T Tolerance() const {
        return std::max(absolute_floor,
                        std::abs(bounding_box_diagonal) * relative_epsilon);
    }
};
```

The absolute floor remains meaningful for a zero-size or very small model. In
production numerical code, the appropriate formula and constants must be derived
from the operation being performed; a tolerance for an area predicate, for example,
may scale differently from a tolerance for a coordinate comparison.

**Step 3 — parameterize the operation:**

```cpp
template <ScalarType T, TolerancePolicy<T> Policy>
inline int RobustSign(T value, const Policy& tolerance_policy) {
    const T tolerance{tolerance_policy.Tolerance()};
    if (value > tolerance) { return 1; }
    if (value < -tolerance) { return -1; }
    return 0;
}
```

`RobustSign` no longer defines what tolerance means; it only requires a policy that
produces a value compatible with `T`. A caller can select `FixedTolerance<double>`
or `ScaledTolerance<double>` without modifying `RobustSign`. Because the concrete
policy type is visible to the compiler, this abstraction can normally be inlined.
That claim should be confirmed by measurement if the function is performance
critical. The policy must also produce a tolerance with the same physical dimension
as `value`; template constraints can verify types and expressions, but not physical
units represented by ordinary arithmetic types.

---

## Part 7 — Choosing an appropriate dispatch mechanism

Policy-based design is not a general replacement for runtime Strategy. The following
questions help determine which mechanism is appropriate:

1. **Is the implementation known at compile time for each use?**
   - If yes, a template parameter and, where useful, a concept are natural choices.
   - If no—for example, the choice comes from user input, configuration, or a
     dynamically loaded component—some form of runtime dispatch is required.

2. **Is the set of runtime alternatives closed and known in advance?**
   - If yes, `std::variant<PolicyA, PolicyB, PolicyC>` with `std::visit` may be
     appropriate. It stores one of a fixed set of types directly and does not require
     virtual dispatch. Its layout, code-size, and dispatch characteristics should
     still be evaluated for the application.
   - If no, an interface with virtual dispatch is usually the appropriate design.
     This supports extension without modifying the context, although deployment and
     ABI requirements determine whether new implementations can be added without
     rebuilding other components.

3. **Is the operation performance-sensitive?**
   - In a hot loop, static dispatch may enable valuable inlining and optimization.
     Measure the alternatives before attributing a performance problem to virtual
     dispatch alone.
   - For an operation performed once per user action or file load, maintainability
     and runtime extensibility usually matter more than dispatch overhead.

In summary, runtime Strategy and policy-based design represent the same separation
of concerns at different binding times. The selection time, extensibility
requirements, ownership model, and measured performance characteristics should
determine the choice.

---

## Part 8 — Costs of policy-based design

- **Generated code and compilation cost.** Distinct template instantiations may
  generate distinct machine code. A large number of policy combinations can increase
  build times and binary size, although link-time folding may remove some duplicates.
- **Diagnostics and complexity.** Concepts generally improve diagnostics compared
  with unconstrained templates or SFINAE, but deeply composed templates can still
  produce difficult errors and expose more implementation detail to users.
- **Definition visibility.** Template definitions generally must be visible at the
  point of instantiation. This commonly moves implementation into headers and causes
  multiple translation units to parse and instantiate the same templates. Explicit
  instantiation can mitigate this for a known set of types.
- **Type proliferation.** Each policy combination is a distinct C++ type. This may
  complicate storage in homogeneous containers, stable ABI boundaries, and APIs that
  should not expose implementation choices.
- **Unnecessary generality.** A variation point should reflect a real requirement.
  Introducing policy parameters speculatively can make an interface harder to use
  without providing a practical benefit.

---

## Part 9 — Exercises

The following exercises progress from a local extension to broader design analysis:

1. **Add a policy.** In the `EuclideanPolicy`/`ManhattanPolicy` example, add a third
   policy, `ChebyshevPolicy` (max of `|dx|`, `|dy|`), and confirm
   `NearestNeighborFinder<ChebyshevPolicy>` compiles with no changes to
   `NearestNeighborFinder` itself.

2. **Apply the tolerance policy.** Introduce the `TolerancePolicy` from Part 6 in a
   geometry library. Update `RobustSign`,
   `NearlyEqualPoint`, and anything that calls them, to take a policy instead of the
   implicit `kTolerance<T>` default. If source compatibility is required, consider a
   default policy:

   ```cpp
   template <ScalarType T, typename Policy = FixedTolerance<T>>
       requires TolerancePolicy<Policy, T>
   ```

3. **Compose independent policies.** Extend a polygon-mesh type with a second
   template parameter for its ear-validity test—for example,
   `BruteForceReflexScan` and `SpatialIndexReflexScan`. Define the interface required
   by both policies and verify that either can be selected without changing the
   mesh's linked-list implementation.

4. **Identify the binding boundary.** Find one runtime `if` or `switch` whose choice
   is actually known at compile time, and one use of runtime dispatch whose choice
   cannot be known until execution. Explain why each belongs on its respective side
   of the boundary.
