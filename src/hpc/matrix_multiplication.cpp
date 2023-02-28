#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

// Function to multiply two matrices
void multiply_matrices(const std::vector<std::vector<double>>& A,
                       const std::vector<std::vector<double>>& B,
                       std::vector<std::vector<double>>& C)
{
    int N = A.size();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0.0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main()
{
    // Set up OpenMP
    int num_threads = 4;
    omp_set_num_threads(num_threads);

    // Define matrix dimensions
    const int N = 1000;

    // Allocate memory for matrices
    std::vector<std::vector<double>> A(N, std::vector<double>(N));
    std::vector<std::vector<double>> B(N, std::vector<double>(N));
    std::vector<std::vector<double>> C(N, std::vector<double>(N));

    // Initialize matrices with random values
    srand(0);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = (double)rand() / RAND_MAX;
            B[i][j] = (double)rand() / RAND_MAX;
        }
    }

    // Multiply matrices and time the operation
    auto start = std::chrono::steady_clock::now();
    multiply_matrices(A, B, C);
    auto end = std::chrono::steady_clock::now();

    // Print the execution time
    std::chrono::duration<double> elapsed_time = end - start;
    std::cout << "Execution time: " << elapsed_time.count() << " seconds" << std::endl;

    return 0;
}
