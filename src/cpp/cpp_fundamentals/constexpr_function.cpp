#include <array>
#include <cassert>
#include <chrono>
#include <iostream>

// ============================================================
// constexpr function rules illustrated here:
//   ✅ can use loops
//   ✅ can use local variables
//   ✅ can be called with non-const args (result is then runtime)
//   ❌ cannot modify non-local variables (no side effects)
//   ❌ cannot call non-constexpr functions
// ============================================================

constexpr long long sum_of_squares(int n) {
    long long total{0};           // local variable  — allowed
    for (int i = 1; i <= n; i++)  // loop            — allowed
        total += static_cast<long long>(i) * i;
    return total;
}

int main() {
    using clock = std::chrono::high_resolution_clock;
    using us = std::chrono::microseconds;

    std::cout << "================================================================\n";
    std::cout << "  constexpr function: compile-time vs runtime evaluation  \n";
    std::cout << "================================================================\n\n";

    // --------------------------------------------------------------------
    // PART 1: constexpr argument -> result is a COMPILE-TIME constant
    // --------------------------------------------------------------------
    constexpr int N{500};
    constexpr long long ct_result{sum_of_squares(N)};  // evaluated at compile time

    // Proof #1: can be used as an array size (requires constant expression)
    std::array<int, sum_of_squares(4)> arr{};                              // sum_of_squares(4) = 30
    static_assert(sum_of_squares(4) == 30, "1^2 + 2^2 + 3^2 + 4^2 = 30");  // compile-time check

    // Proof #2: static_assert -> only works with constant expressions
    static_assert(ct_result > 0, "ct_result is a compile-time constant");

    std::cout << "[COMPILE-TIME] sum_of_squares(" << N << ") = " << ct_result << "\n";
    std::cout << "  Proven compile-time: used in static_assert and array size\n\n";

    // --------------------------------------------------------------------
    // PART 2: non-const argument -> result computed at RUNTIME
    // --------------------------------------------------------------------
    int n{N};  // same value -> but NOT a constant expression
    long long rt_result{sum_of_squares(n)};

    // These would FAIL TO COMPILE -> rt_result is not a constant expression:
    // static_assert(rt_result == ct_result, "...");  // ❌ compiler error
    // std::array<int, rt_result> arr2{};             // ❌ compiler error

    std::cout << "[RUNTIME]      sum_of_squares(n) where n=" << n << " = " << rt_result << "\n";
    std::cout << "  Cannot use in static_assert or as array size\n\n";

    // Same value — different timing
    assert(ct_result == rt_result);
    std::cout << "Results match: " << (ct_result == rt_result ? "YES" : "NO")
              << " (same value, different evaluation time)\n\n";

    // ----------------------------------------------------------
    // PART 3: Performance measurement
    //
    // volatile int forces the compiler to treat runtime_n as truly
    // unknown — prevents it from optimising the call away
    // ----------------------------------------------------------
    const int REPS{200'000};
    long long sink{0};  // accumulate results to prevent dead-code elimination

    // Compile-time path: ct_result is a constant baked into the binary
    auto t1{clock::now()};
    for (int i = 0; i < REPS; i++) {
        sink += ct_result;  // just loading a constant
    }
    auto t2{clock::now()};

    // Runtime path: volatile prevents the compiler caching the result
    volatile int runtime_n{N};
    auto t3{clock::now()};
    for (int i = 0; i < REPS; i++) {
        sink += sum_of_squares(runtime_n);  // inner loop of N iterations each call
    }
    auto t4{clock::now()};

    auto ct_time{std::chrono::duration_cast<us>(t2 - t1).count()};
    auto rt_time{std::chrono::duration_cast<us>(t4 - t3).count()};

    std::cout << "=================================================\n";
    std::cout << "  Performance (" << REPS << " iterations, N=" << N << ")\n";
    std::cout << "=================================================\n";
    std::cout << "  Compile-time constant (load value): " << ct_time << " us\n";
    std::cout << "  Runtime computation  (loop N=500):  " << rt_time << " us\n";

    // Print the speedup
    if (ct_time > 0) {
        std::cout << "  Speedup: " << static_cast<double>(rt_time) / ct_time << "x\n";
    } else {
        std::cout << "  Compile-time so fast it rounds to 0 us\n";
    }
    std::cout << "\n  Work done per iteration:\n";
    std::cout << "    Compile-time: 1 load  (result pre-computed)\n";
    std::cout << "    Runtime:      " << N << " multiply-adds (inner loop runs every call)\n";

    std::cout << "\n(sink=" << sink << ")\n";  // prevent dead-code elimination
}
