// A simple program to compute the sum of reciprocals of first 100 million integers using omp

#include <chrono>
#include <iostream>
#include <omp.h>

constexpr int N{100'000'000};

int main() {
    // Take the number of threads to use as input
    int num_threads{1};
    std::cout << "Enter number of threads: ";
    std::cin >> num_threads;
    omp_set_num_threads(num_threads);

    double sum{0.0};
    auto start_time = std::chrono::high_resolution_clock::now();

    // create threads to execute the loop in parallel and produce final sum
    // by combining results from all threads
    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= N; ++i) {
        sum += 1.0 / i;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    ).count();

    std::cout << "The sum of 1/1 + 1/2 + ... + 1/" << N << " is " << sum << std::endl;
    std::cout << "Computed in " << duration_ms << " ms using " << omp_get_max_threads() <<
        " threads." << std::endl;

    return 0;
}
