#include <cmath>
#include <iostream>

//---------------------------------------------------------------------------
// Matrix multiplication on the CPU
//---------------------------------------------------------------------------
// Computes C = A * B for two N×N matrices stored in row-major order.
//
// Row-major layout: element (row, col) lives at index [row * N + col].
// So row 0 is indices 0..N-1, row 1 is N..2N-1, and so on.
//
// All three pointers are flat 1D arrays — the 2D structure is implicit.
// A and B are read-only (const); C is write-only (unread inside this function).
void matmul_cpu(int N, const float* A, const float* B, float* C) {
    // iterate over rows of A (and C)
    for (int row = 0; row < N; ++row) {
        // iterate over columns of B (and C)
        for (int col = 0; col < N; ++col) {
            // C[row][col] = dot product of row `row` of A with column `col` of B
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                // A[row][k]  -> A[row * N + k]   (step along a row: k moves by 1)
                // B[k][col]  -> B[k   * N + col] (step along a column: k moves by N)
                sum += A[row * N + k] * B[k * N + col];
            }
            // write the result into C[row][col]
            C[row * N + col] = sum;
        }
    }
}

int main() {
    constexpr int N = 4;  // matrix dimension: N×N

    // A is a 4×4 matrix with values 1->16, stored row by row
    float A[N * N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    // B is the 4×4 identity matrix (1s on the diagonal, 0s elsewhere)
    // Multiplying any matrix by the identity returns the original: A * I = A
    // This makes verification trivial -> C should equal A exactly
    float B[N * N] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    // Zero-initialize C -> the = {} syntax value-initializes all elements to 0
    float C[N * N] = {};

    matmul_cpu(N, A, B, C);

    // Print C row by row -> should match A exactly
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            std::cout << C[row * N + col] << " ";
        }
        std::cout << '\n';  // newline after each row
    }
}
