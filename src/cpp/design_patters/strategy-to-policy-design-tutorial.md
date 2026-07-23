# Strategy → Policy-Based Design: A C++ Tutorial

## Part 1 — The problem Strategy solves

Every version of this pattern — GoF's runtime Strategy, Alexandrescu's policy-based
design — answers the same question:

> **A class has one algorithm that varies, and everything else about the class stays
> the same. How do you let the algorithm vary without rewriting the class for every
> variant?**

Concretely, in your world: you might want a `Point`-to-`Point` distance function that
sometimes means Euclidean distance and sometimes means Manhattan distance, or a
tolerance check that's sometimes a fixed epsilon and sometimes scaled to the model's
bounding box. The *shape* of the operation is the same everywhere it's used; only the
*rule* changes.

Without a pattern, you get one of two bad outcomes:
- A giant `if (mode == kEuclidean) ... else if (mode == kManhattan) ...` branch
  duplicated at every call site, or
- A class that hardcodes one specific rule, forcing a full rewrite (or copy-paste
  fork) every time a new rule shows up.

Strategy — in either its runtime or compile-time form — extracts "the rule that
varies" into its own first-class thing, and has the surrounding code depend on *that*
instead of hardcoding it.

---

## Part 2 — Classic Strategy (runtime, GoF form)

The textbook version: define an abstract interface for the varying behavior, and
inject a concrete implementation at runtime.

```cpp
#include <memory>
#include <cmath>

struct Point { double x{}, y{}; };

// The "strategy" interface — the thing that varies
class DistanceStrategy {
public:
    virtual ~DistanceStrategy() = default;
    virtual double Distance(const Point& a, const Point& b) const = 0;
};

class EuclideanDistance : public DistanceStrategy {
public:
    double Distance(const Point& a, const Point& b) const override {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }
};

class ManhattanDistance : public DistanceStrategy {
public:
    double Distance(const Point& a, const Point& b) const override {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
};

// The class that USES a strategy, without knowing which one
class NearestNeighborFinder {
public:
    explicit NearestNeighborFinder(std::unique_ptr<DistanceStrategy> strategy)
        : strategy_{std::move(strategy)} {}

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

**What you bought:** `NearestNeighborFinder` never mentions Euclidean or Manhattan by
name. You can swap the strategy at runtime — read it from a config file, let a user
pick it from a dropdown, change it mid-program. Adding a new distance metric means
writing one new class; `NearestNeighborFinder` never changes (Open/Closed in its
original, textbook form).

**What it costs:**
- `strategy_->Distance(...)` is a **virtual call** — an indirect jump through a
  vtable. The compiler cannot inline it, because it doesn't know at compile time
  which override will run.
- `std::unique_ptr<DistanceStrategy>` is a **heap allocation** and **pointer
  indirection** just to hold "which rule to use."
- If `FindNearest` is called on a candidate list of 10 points, this overhead is
  noise. If it's called in the inner loop of a mesh algorithm touching millions of
  points, the indirect call becomes a real, measurable cost — and worse, it defeats
  auto-vectorization, because the compiler can't reason across an opaque virtual call.

This is the exact tension from our composition-over-inheritance conversation, now
concretely quantified: Strategy gives you runtime flexibility by spending an
indirection at every call. In a hot loop, that's a bad trade for something that
never actually changes at runtime.

---

## Part 3 — The insight: is the "variation" actually a runtime fact?

Ask this question about the Euclidean/Manhattan example: **does any single program
run ever need to switch distance metrics mid-execution?** Almost never. You typically
know which metric you want *before you compile* — it's baked into the algorithm
you're implementing, not a runtime user choice.

If the variation point is known at compile time, paying a runtime indirection cost
for it is pure waste. This is the moment to reach for **policy-based design**:
same conceptual pattern (inject the varying behavior), different mechanism
(template parameter instead of virtual base class).

---

## Part 4 — Policy-based design: the same Strategy, resolved at compile time

```cpp
#include <cmath>
#include <concepts>

struct Point { double x{}, y{}; };

// A "policy" is just a type with the expected interface — enforced with a concept,
// not an inheritance requirement.
template <typename P>
concept DistancePolicy = requires(P policy, Point a, Point b) {
    { policy.Distance(a, b) } -> std::convertible_to<double>;
};

struct EuclideanPolicy {
    double Distance(const Point& a, const Point& b) const {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }
};

struct ManhattanPolicy {
    double Distance(const Point& a, const Point& b) const {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
};

// The class that USES a policy — the policy is a template parameter, not a
// runtime-injected object.
template <DistancePolicy Policy>
class NearestNeighborFinder {
public:
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
    Policy policy_{};  // empty struct -> zero size, zero runtime cost
};

// Usage: the "strategy choice" happens at the call site, at compile time
NearestNeighborFinder<EuclideanPolicy> euclidean_finder;
NearestNeighborFinder<ManhattanPolicy> manhattan_finder;
```

**What changed, mechanically:**
- `policy_.Distance(...)` is now a call on a **concrete type known at compile time**
  — the compiler can inline it completely. In an optimized build, the call
  disappears; only the arithmetic remains.
- `EuclideanPolicy`/`ManhattanPolicy` are empty structs — no vtable, no heap
  allocation, `sizeof(Policy) == 1` (or genuinely 0 with `[[no_unique_address]]`).
- The `DistancePolicy` concept plays the exact role the abstract base class played
  before: it's still an *interface* — "must have a `Distance(Point, Point) -> double`
  method" — just checked by the compiler at instantiation instead of enforced by
  inheritance.

**What you gave up:** you can no longer choose the policy at runtime. If your program
genuinely needs to switch strategies based on user input read at runtime, this
version can't do that — you'd need either the GoF version, or `std::variant` over a
closed set of policies with `std::visit` dispatching once at a boundary (more on this
tradeoff in Part 7).

This is the whole idea of policy-based design in one example: **same conceptual
pattern as Strategy, same benefit (decoupled, swappable behavior), moved from a
runtime mechanism (virtual dispatch) to a compile-time mechanism (templates)** —
because the thing that varies was actually a compile-time fact all along.

---

## Part 5 — Multiple orthogonal policies (the Alexandrescu-style "policy host")

The real power of policy-based design shows up when a class has **more than one
independent axis of variation**, and you want to mix and match them without an
explosion of subclasses.

Imagine a `Grid` class that varies along two *independent* dimensions: how it stores
its cells, and how out-of-bounds access is handled.

```cpp
// Storage policy axis
struct DenseStorage {
    // stores every cell explicitly, e.g. std::vector<Cell>
};
struct SparseStorage {
    // stores only non-empty cells, e.g. std::unordered_map<Index, Cell>
};

// Bounds policy axis
struct ClampBounds {
    // out-of-range access clamps to the nearest valid index
};
struct WrapBounds {
    // out-of-range access wraps around (periodic boundary)
};

// The host class takes BOTH policies as independent template parameters
template <typename StoragePolicy, typename BoundsPolicy>
class Grid : private StoragePolicy, private BoundsPolicy {
    // ... implementation delegates storage decisions to StoragePolicy,
    //     index-clamping decisions to BoundsPolicy
};

// Every combination is available, with NO combinatorial subclassing:
Grid<DenseStorage, ClampBounds>  grid_a;
Grid<SparseStorage, WrapBounds>  grid_b;
Grid<DenseStorage, WrapBounds>   grid_c;
Grid<SparseStorage, ClampBounds> grid_d;
```

**Why this matters, and why inheritance genuinely can't do this cleanly:** with
classic single-inheritance Strategy, if you wanted both a storage strategy *and* a
bounds strategy to vary independently, you'd need either multiple inheritance (messy,
diamond-problem-prone) or you'd collapse both axes into one hierarchy
(`DenseClampGrid`, `SparseWrapGrid`, ...) — which is exactly the combinatorial
explosion that composition/policies are supposed to prevent. Independent template
parameters give you the full cross-product of combinations for free, resolved and
inlined at compile time, with zero runtime cost for any of it.

(Note: inheriting from empty policy structs — `private StoragePolicy` — is the Empty
Base Optimization; it lets the compiler eliminate their storage entirely when they
carry no state, unlike a `Policy member_;` field which may still cost a byte per
policy depending on your compiler/standard version.)

---

## Part 6 — Applying this to your own code: fixing the tolerance problem

Recall the tolerance discussion from earlier in this thread: `kTolerance<T>` in
your `geometry.hpp` is a single global constant, which breaks down across different
coordinate scales. **This is a policy-based design problem wearing a disguise.**
Let's actually build the fix.

**Step 1 — name the interface every tolerance policy must satisfy:**

```cpp
template <typename P, typename T>
concept TolerancePolicy = requires(P policy, T value) {
    { policy.Tolerance() } -> std::convertible_to<T>;
};
```

**Step 2 — write two concrete policies for two real scenarios:**

```cpp
// Scenario 1: you know the fixed tolerance up front (current behavior, made explicit)
template <ScalarType T>
struct FixedTolerance {
    T value{static_cast<T>(1e-9)};
    T Tolerance() const { return value; }
};

// Scenario 2: tolerance scales with the model's bounding box diagonal —
// solves the "microns vs. millimeters" problem from earlier
template <ScalarType T>
struct ScaledTolerance {
    T bounding_box_diagonal{static_cast<T>(1.0)};
    T relative_epsilon{static_cast<T>(1e-9)};
    T Tolerance() const { return bounding_box_diagonal * relative_epsilon; }
};
```

**Step 3 — make `RobustSign` (and everything built on it) take a policy instead of a
bare default:**

```cpp
template <ScalarType T, TolerancePolicy<T> Policy>
inline int RobustSign(T value, const Policy& tolerance_policy) {
    const T tolerance{tolerance_policy.Tolerance()};
    if (value > tolerance) { return 1; }
    if (value < -tolerance) { return -1; }
    return 0;
}
```

**What you now have, that you didn't before:**
- `RobustSign` no longer hardcodes what "tolerance" means — it just requires
  *something* that can produce a `T`.
- Swapping from fixed to scaled tolerance is a **one-line change at the call site**
  (`FixedTolerance<double>{}` → `ScaledTolerance<double>{diagonal, 1e-9}`), with zero
  changes to `RobustSign` itself, `TriangleOrientation`, or anything downstream —
  exactly the Open/Closed benefit, achieved with templates instead of inheritance.
- Because both policies are concrete types resolved at compile time, there's no
  runtime cost over your current hardcoded version — you get the flexibility for
  free.

This is precisely the exercise you'd want to be able to walk through live in a
design interview: **take a hardcoded value, recognize it as an unstated policy,
extract the policy, and show that extraction costs nothing at runtime.**

---

## Part 7 — Decision framework: when do you still want runtime Strategy?

Policy-based design isn't a strict upgrade — it's the right tool only when the
condition from Part 3 holds. Ask, in order:

1. **Is the variation known at compile time, for any given translation unit?**
   - Yes → policy-based design (template parameter + concept).
   - No (must be chosen at runtime — user input, config file, plugin loaded at
     startup) → you need runtime dispatch of *some* form. Go to question 2.

2. **Is the set of possible variants closed and known in advance (even if the
   *choice* among them happens at runtime)?**
   - Yes → prefer `std::variant<PolicyA, PolicyB, PolicyC>` + `std::visit`. You still
     get exhaustiveness checking and avoid heap allocation/vtables — the dispatch
     cost is one runtime branch at the `visit` call, not scattered virtual calls
     throughout the algorithm.
   - No (truly open-ended — a plugin system where new strategies can be added
     without recompiling) → classic GoF Strategy with virtual dispatch is the
     correct, and really only, choice.

3. **Is this call site actually hot?** (called in a tight loop over large data,
   vs. called once per user action or once per file load)
   - Hot path → the compile-time cost of virtual dispatch is worth avoiding;
     lean hard toward policies.
   - Cold path (e.g., choosing a file importer once per file load) → the
     indirection cost of classic Strategy is genuinely irrelevant; don't over-engineer
     by templating something called once.

**The one-sentence version:** *runtime Strategy and policy-based design solve the
same problem; which one is correct is entirely determined by when the variation
point is known and how hot the call site is — not by which one feels more "modern."*

---

## Part 8 — Pitfalls of policy-based design (so you don't oversell it)

Worth knowing these, because an interviewer who's satisfied you understand the
benefit will often probe for the cost:

- **Code bloat.** Every distinct template instantiation (`Grid<DenseStorage,
  ClampBounds>` vs. `Grid<SparseStorage, WrapBounds>`) generates its own compiled
  code. Heavy policy combinations across a large codebase can meaningfully increase
  binary size and compile time — the mirror image of the Pimpl motivation from
  earlier.
- **Error messages.** Concept constraints help a lot here versus raw SFINAE, but
  deeply nested policy-based templates can still produce long, hard-to-read compiler
  errors when a policy doesn't satisfy its concept correctly.
- **Header-only pressure.** Templates generally must be visible at the point of
  instantiation, which pushes implementation into headers (exactly what your
  `geometry-kernel` already embraces) — fine for a library like yours, but it's a
  real architectural consequence, not a free lunch: every translation unit that uses
  the template re-compiles it.
- **You can over-genericize.** Not every varying value needs to become a template
  parameter. If something varies rarely, isn't in a hot path, and the "variants" are
  genuinely open-ended, forcing it into a policy is speculative generality — the same
  smell from the "program to the interface" conversation, just wearing a template
  instead of a virtual base class.

---

## Part 9 — Exercises to build intuition further

Try these against your own repos, in order of difficulty:

1. **Warm-up:** In the `EuclideanPolicy`/`ManhattanPolicy` example above, add a third
   policy, `ChebyshevPolicy` (max of `|dx|`, `|dy|`), and confirm
   `NearestNeighborFinder<ChebyshevPolicy>` compiles with no changes to
   `NearestNeighborFinder` itself.

2. **Applied to your code:** Actually make the `TolerancePolicy` change from Part 6
   in `geometry-kernel` or `polygon-triangulation`. Update `RobustSign`,
   `NearlyEqualPoint`, and anything that calls them, to take a policy instead of the
   implicit `kTolerance<T>` default. (Tip: give the policy a defaulted template
   parameter — `template <ScalarType T, TolerancePolicy<T> Policy = FixedTolerance<T>>`
   — so existing call sites keep compiling unchanged, which is itself a good example
   of Open/Closed in practice.)

3. **Harder — multiple orthogonal policies:** Extend `PolygonMesh<T>` with a second,
   independent template parameter for the *ear-validity test strategy* — e.g., a
   `BruteForceReflexScan` policy (what it does today) vs. a hypothetical
   `SpatialIndexReflexScan` policy (the AABB-accelerated version from the TODO you
   already flagged). Sketch just the interface both policies would need to satisfy —
   you don't need to implement the spatial index for this exercise, just prove the
   two policies are swappable without touching the linked-list machinery.

4. **Stretch — recognize the boundary:** Identify one place in Kynema-FMB (or any
   codebase you know well) where a runtime `if`/`switch` on a "mode" or "type" enum
   is actually gating a compile-time-known choice, and one place where runtime
   dispatch is *correctly* used because the choice genuinely can't be known until
   runtime. Being able to point to a real example of each, and explain *why* each one
   is on the correct side of the line, is a strong signal in a design interview.
