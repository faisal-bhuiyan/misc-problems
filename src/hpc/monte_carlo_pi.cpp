/*
 * In this program, we use a Monte Carlo simulation to estimate the value of pi by
 * generating a large number of random points inside a square with side length 2.0
 * and counting the number of points that fall inside the unit circle inscribed in
 * the square. The ratio of the number of points inside the circle to the total number
 * of points is an estimate of the area of the circle relative to the area of the
 * square, which is pi/4. Multiplying this ratio by 4 gives an estimate of pi.
 */

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

#include <omp.h>

// Function to estimate the value of pi using a Monte Carlo simulation
double estimate_pi(int num_samples) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    int count {0};

    // Compute the number of points inside the unit circle by dividing the
    // work among threads and making sure the work is evenly distributed
    omp_set_num_threads(10);

    #pragma omp parallel for reduction(+:count) schedule(static)
    for (int i = 0; i < num_samples; ++i) {
        double x{dist(gen)};
        double y{dist(gen)};
        if (std::sqrt(x*x + y*y) <= 1.0) {
            ++count;
        }
    }

    // Compute the value of pi
    return 4.0 * count / num_samples;
}

int main() {
    int num_samples{10'000'000};

    auto start = std::chrono::steady_clock::now();

    double pi = estimate_pi(num_samples);

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Print the estimated value of pi and the elapsed time
    std::cout << "Estimated value of pi: " << pi << std::endl;
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;

    return 0;
}
