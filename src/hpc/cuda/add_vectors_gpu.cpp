#include <cmath>
#include <iostream>

//---------------------------------------------------------------------------
// v1 - single thread
//---------------------------------------------------------------------------

// Kernel function to add the elements of two arrays
__global__ void add(int n, float* x, float* y) {
    for (int i = 0; i < n; i++) {
        y[i] = x[i] + y[i];
    }
}

int main(void) {
    int N = 1 << 20;  // 1 million elements
    float *x, *y;

    // Allocate Unified Memory –> accessible from CPU or GPU
    cudaMallocManaged(&x, N * sizeof(float));
    cudaMallocManaged(&y, N * sizeof(float));

    // initialize x and y arrays on the host --> 1.0f and 2.0f
    for (int i = 0; i < N; i++) {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }

    // Run kernel on 1M elements on the GPU -> 1 thread, 1 block
    add<<<1, 1>>>(N, x, y);

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();

    // Check for errors (all values should be 3.0f)
    float maxError = 0.0f;
    for (int i = 0; i < N; i++) {
        maxError = std::fmax(maxError, std::fabs(y[i] - 3.0f));
    }
    std::cout << "Max error: " << maxError << std::endl;

    // Free memory
    cudaFree(x);
    cudaFree(y);

    return 0;
}

//---------------------------------------------------------------------------
// v2 - grid of threads
//---------------------------------------------------------------------------

// Kernel function to add the elements of two arrays
__global__ void add(int n, float* x, float* y) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;  // thread index
    int stride = blockDim.x * gridDim.x;                // stride
    for (int i = index; i < n; i += stride) {
        y[i] = x[i] + y[i];
    }
}

int main(void) {
    int N = 1 << 20;
    float *x, *y;

    // Allocate Unified Memory – accessible from CPU or GPU
    cudaMallocManaged(&x, N * sizeof(float));
    cudaMallocManaged(&y, N * sizeof(float));

    // initialize x and y arrays on the host
    for (int i = 0; i < N; i++) {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }

    // Run kernel on 1M elements on the GPU
    int blockSize = 256;                              // 256 threads per block
    int numBlocks = (N + blockSize - 1) / blockSize;  // number of blocks -> ceil(N / blockSize)
    add<<<numBlocks, blockSize>>>(N, x, y);

    // Wait for GPU to finish before accessing on host
    cudaDeviceSynchronize();

    // Check for errors (all values should be 3.0f)
    float maxError = 0.0f;
    for (int i = 0; i < N; i++) {
        maxError = std::fmax(maxError, std::fabs(y[i] - 3.0f));
    }
    std::cout << "Max error: " << maxError << std::endl;

    // Free memory
    cudaFree(x);
    cudaFree(y);

    return 0;
}

//---------------------------------------------------------------------------
// v3 - grid of threads with error checking
//---------------------------------------------------------------------------

// ── Error-checking macro ────────────────────────────────────────────────────
#define CUDA_CHECK(call)                                                                          \
    do {                                                                                          \
        cudaError_t err = (call);                                                                 \
        if (err != cudaSuccess) {                                                                 \
            std::fprintf(                                                                         \
                stderr, "CUDA error at %s:%d — %s\n", __FILE__, __LINE__, cudaGetErrorString(err) \
            );                                                                                    \
            std::exit(EXIT_FAILURE);                                                              \
        }                                                                                         \
    } while (0)

// ── Kernel ──────────────────────────────────────────────────────────────────
// x is read-only: mark const so the compiler can place it in read-only cache
__global__ void add(int n, const float* __restrict__ x, float* __restrict__ y) {
    const int index = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = blockDim.x * gridDim.x;
    for (int i = index; i < n; i += stride) {
        y[i] = x[i] + y[i];
    }
}

// ── Helpers ─────────────────────────────────────────────────────────────────
// Ceiling division — keeps launch config readable
constexpr int cdiv(int n, int block) {
    return (n + block - 1) / block;
}

int main() {
    constexpr int N = 1 << 20;  // 1M elements
    constexpr int blockSize = 256;
    constexpr float X_VAL = 1.0f;
    constexpr float Y_VAL = 2.0f;

    // ── Host arrays ─────────────────────────────────────────────────────────
    // Use std::vector for RAII; raw new[] would need manual delete[]
    std::vector<float> h_x(N, X_VAL);
    std::vector<float> h_y(N, Y_VAL);

    // ── Device arrays ───────────────────────────────────────────────────────
    // Explicit device memory teaches the real host/device boundary
    float *d_x{}, *d_y{};
    CUDA_CHECK(cudaMalloc(&d_x, N * sizeof(float)));
    CUDA_CHECK(cudaMalloc(&d_y, N * sizeof(float)));

    CUDA_CHECK(cudaMemcpy(d_x, h_x.data(), N * sizeof(float), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_y, h_y.data(), N * sizeof(float), cudaMemcpyHostToDevice));

    // ── Launch ───────────────────────────────────────────────────────────────
    add<<<cdiv(N, blockSize), blockSize>>>(N, d_x, d_y);
    CUDA_CHECK(cudaGetLastError());       // catch bad launch params
    CUDA_CHECK(cudaDeviceSynchronize());  // wait + catch kernel errors

    // ── Copy result back & verify ────────────────────────────────────────────
    CUDA_CHECK(cudaMemcpy(h_y.data(), d_y, N * sizeof(float), cudaMemcpyDeviceToHost));

    float maxError = 0.0f;
    for (int i = 0; i < N; ++i) {
        maxError = std::fmax(maxError, std::fabs(h_y[i] - 3.0f));
    }
    std::cout << "Max error: " << maxError << '\n';

    // ── Cleanup ──────────────────────────────────────────────────────────────
    CUDA_CHECK(cudaFree(d_x));
    CUDA_CHECK(cudaFree(d_y));
    // h_x, h_y freed automatically by vector destructor
}
