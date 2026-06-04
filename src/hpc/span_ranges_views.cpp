/**
 * @file span_ranges_views.cpp
 * @brief Concise examples connecting std::span, std::ranges, std::views
 *
 * Build:
 *   clang++ -std=c++20 -O2 span_ranges_views.cpp -o span_ranges_views
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <ranges>
#include <span>
#include <vector>

//---------------------------------------------------------------------------
// std::span —> non-owning view over contiguous memory
//---------------------------------------------------------------------------

/**
 * @brief Non-owning view over a contiguous sequence of objects (`std::span<T>`).
 *
 * `std::span` is the standard replacement for the conventional `(T* data, std::size_t n)` pair.
 * It provides a uniform interface regardless of how the underlying storage is owned or allocated.
 *
 * @par Representation
 * A span stores exactly two words internally: a pointer to the first element and the number of
 * elements. It performs no heap allocation and makes no copy of the data -> it is a lightweight
 * descriptor over memory that already exists.
 *
 * @par Construction
 * A span is implicitly constructible from any contiguous range the standard recognizes —> including
 * `std::vector<T>`, raw arrays, and `std::array<T, N>`. It can also be constructed from an
 * explicit pointer and length. `subspan(offset, count)` produces a view/slice of an interior segment
 * without copying.
 *
 * @par Comparison with `std::vector<T>&`
 * A `std::vector<T>&` parameter constrains the caller to supply an owning `std::vector`. A
 * `std::span<T>` parameter imposes no such constraint: the memory may reside in a vector, a
 * stack array, a raw heap allocation, or any other contiguous region. The function body observes
 * the elements only through the span and is decoupled from the ownership model of the caller.
 *
 * @par Relevance to numerical and HPC code
 * Numerical kernels and libraries (BLAS-style interfaces, `std::linalg`) have always operated on
 * pointer-plus-length descriptors for precisely this reason: the kernel should not require the
 * caller to store data in a specific container. `std::span` formalises that convention in the
 * C++20 type system.
 */
void scale(std::span<double> data, double factor) {
    for (auto& x : data) {
        x *= factor;
    }
}

void example_span() {
    //-------------------------------------
    // Example 1: scaling a vector
    //-------------------------------------
    std::vector<double> v{1, 2, 3, 4, 5, 6, 7, 8};
    std::cout << "original vector: ";
    for (auto x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // scale the whole vector by 2
    scale(v, 2.);
    // scale the elements [2, 3, 4, 5] by 10
    scale(std::span(v).subspan(2, 4), 10.);

    std::cout << "scaled vector: ";
    for (auto x : v) {
        std::cout << x << " ";  // 2 4 60 80 100 120 14 16
    }
    std::cout << "\n";

    //-------------------------------------
    // Example 2: scaling a raw array
    //-------------------------------------
    // span over a raw array —> using same function
    double arr[]{9, 10, 11, 12};
    std::cout << "original array: ";
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    // scale the array by 0.5
    scale(arr, 0.5);
    std::cout << "scaled array: ";
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

//---------------------------------------------------------------------------
// std::ranges —> algorithms that take a whole range, not iterator pairs
//---------------------------------------------------------------------------

/**
 * @brief `std::ranges` — range-based algorithm overloads that accept whole ranges
 *        instead of iterator pairs.
 *
 * Before C++20, every standard algorithm required an explicit begin/end pair:
 * @code
 *   std::sort(v.begin(), v.end());
 * @endcode
 *
 * The `std::ranges` versions accept any type satisfying the `std::ranges::range`
 * concept directly, eliminating the boilerplate without changing the generated code:
 * @code
 *   std::ranges::sort(v);
 * @endcode
 *
 * Because `std::span<T>` satisfies `std::ranges::contiguous_range`, the same
 * algorithms work uniformly over vectors, arrays, spans, and any other conforming
 * range type.
 */
void example_ranges() {
    std::vector<double> v{5., 3., 8., 1., 4.};
    std::cout << "original vector: ";
    for (auto x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // pre-C++20 style — explicit iterator pair
    // std::sort(v.begin(), v.end());

    // C++20 ranges style —> identical codegen, no iterator boilerplate
    std::ranges::sort(v);
    std::cout << "sorted vector using ranges: ";
    for (auto x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // std::span satisfies contiguous_range, so ranges algorithms work on it too
    // Mutations through the span are visible in the original vector
    std::vector<double> data{4., 1., 7., 2.};
    std::cout << "data (unsorted): ";
    for (auto x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    std::span<double> s(data);
    std::ranges::sort(s);
    std::cout << "data (sorted via span using ranges): ";
    for (auto x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // Other algorithms follow the same pattern —> no iterator pairs needed
    auto it = std::ranges::find(v, 3.);
    auto count = std::ranges::count(v, 3.);
    auto min_value = std::ranges::min(v);
    auto max_value = std::ranges::max(v);
    std::cout << "find(3):  " << *it << "\n";
    std::cout << "count(3): " << count << "\n";
    std::cout << "min:      " << min_value << "\n";
    std::cout << "max:      " << max_value << "\n";
}

//---------------------------------------------------------------------------
// std::views —> lazy pipelines over ranges
//---------------------------------------------------------------------------

/**
 * @brief `std::views` — lazy, composable adaptors over ranges.
 *
 * A view adaptor wraps a range and transforms or filters its elements on demand.
 * No work is performed and no memory is allocated until the pipeline is iterated.
 *
 * Adaptors are composed with the pipe operator `|`. Each stage produces a new
 * view object; the original data is never copied or modified:
 * @code
 *   auto pipeline = v | std::views::filter(pred) | std::views::transform(fn);
 *   for (auto x : pipeline) { ... }   // computation happens here
 * @endcode
 *
 * Views that generate sequences from scratch (`std::views::iota`) avoid
 * allocating a container entirely — the sequence exists only as a view object
 * on the stack.
 *
 * @note `std::views::stride` (every Nth element) is available from C++23.
 *       In C++20 use `std::views::iota` with a step in a transform, or
 *       compose `filter` with a modulo predicate.
 */
void example_views() {
    std::vector<double> v(16);
    std::iota(v.begin(), v.end(), 0.);  // 0, 1, 2, ..., 15

    // Filter to even values then square each —> nothing runs until the loop
    auto evens = std::views::filter([](double x) {
        return static_cast<int>(x) % 2 == 0;
    });
    auto square = std::views::transform([](double x) {
        return x * x;
    });

    for (auto x : v | evens | square) {
        std::cout << x << " ";  // 0 4 16 36 64 100 144 196
    }
    std::cout << "\n";

    // take / drop —> describe subranges without copying or using subspan
    auto first4 = v | std::views::take(4);
    auto mid4 = v | std::views::drop(4) | std::views::take(4);

    // iota view —> generates indices lazily i.e. no vector allocation
    auto squares = std::views::iota(0, 16) | std::views::transform([](int i) {
                       return i * i;
                   });

    for (auto x : squares) {
        std::cout << x << " ";  // 0 1 4 9 16 25 ...
    }
    std::cout << "\n";
}

//---------------------------------------------------------------------------
// Connecting all three —> numerical function over a lazily-filtered,
// non-owning view of a subrange
//---------------------------------------------------------------------------

/**
 * @brief Combining `std::span`, `std::ranges`, and `std::views`.
 *
 * A function constrained on `std::ranges::contiguous_range` accepts any
 * contiguous sequence — `std::vector`, raw array, `std::span`, or a
 * `subspan` of a larger buffer — without requiring a specific container type.
 * Views compose with the range inside the function body just as they would
 * with a plain vector.
 */

/// Sums the positive elements of any contiguous range of doubles
template <std::ranges::contiguous_range R>
double sum_positive(R&& data) {
    double acc = 0.;
    for (auto x : data | std::views::filter([](double v) {
                      return v > 0.;
                  }))
        acc += x;
    return acc;
}

void example_combined() {
    std::vector<double> data{-3., 1., -1., 4., -2., 9., -5., 2.};

    std::cout << sum_positive(data) << "\n";  // 16  — whole vector

    std::cout << sum_positive(std::span(data).subspan(2, 4)) << "\n";  // 13  — middle 4, no copy

    double arr[]{-1., 5., -2., 3.};
    std::cout << sum_positive(arr) << "\n";  // 8   — raw array
}

int main() {
    std::cout << "── span ──\n";
    example_span();
    std::cout << "── ranges ──\n";
    example_ranges();
    std::cout << "── views ──\n";
    example_views();
    std::cout << "── combined ──\n";
    example_combined();
}
