// matrix_move.cpp: Faisal Bhuiyan
// Description: Demonstrating move semantics with a Matrix class. Shows that returning a large Matrix
//              by value uses move construction rather than copy, and how NRVO may elide even the
//              move when the compiler can optimise further.

#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

/*
 * A Matrix may be very large and expensive to copy. The solution is to
 * give Matrix a move constructor: a cheap O(1) operation that steals the
 * heap-allocated data from the source, leaving it in a valid but empty
 * state. The compiler can then return a Matrix from operator+() by moving
 * rather than copying — no manual memory management required.
 *
 * Modern compilers go further still: Named Return Value Optimisation (NRVO)
 * may eliminate even the move, constructing the result directly in the
 * caller's storage. This is strictly better than moving. The move constructor
 * is the safety net — it guarantees cheap return even when NRVO cannot apply.
 *
 * To observe which constructors fire, compile in two ways:
 *   Normal:                g++ -std=c++17 -O0 matrix_move.cpp
 *   Disable elision:       g++ -std=c++17 -O0 -fno-elide-constructors matrix_move.cpp
 *
 * Because we manage a raw resource (the heap array), we must follow the
 * Rule of Five — defining all five special member functions:
 *   1. Destructor
 *   2. Copy constructor     — deep copy,      O(rows * cols)
 *   3. Copy assignment
 *   4. Move constructor     — pointer steal,  O(1)
 *   5. Move assignment
 */

using hires_clock = std::chrono::high_resolution_clock;
using micros_t = std::chrono::microseconds;

// ===============================================================================
// Matrix: owns a heap-allocated rows*cols array of doubles.
// All five special members are defined and traced so we can
// observe exactly which constructor fires in each scenario.
// ===============================================================================
class Matrix {
public:
    // -- Constructor --
    Matrix(int rows, int cols)
        : rows_{rows}, cols_{cols}, data_{new double[static_cast<size_t>(rows * cols)]{}} {
        std::cout << "    [constructed]  " << describe() << "\n";
    }

    // -- Destructor --
    ~Matrix() {
        std::cout << "    [destroyed]    " << describe() << "\n";
        delete[] data_;
    }

    // -- Copy constructor: deep copy — O(rows * cols) --
    // Every element must be duplicated. Expensive for large matrices.
    Matrix(const Matrix& other)
        : rows_{other.rows_},
          cols_{other.cols_},
          data_{new double[static_cast<size_t>(other.rows_ * other.cols_)]} {
        std::copy(other.data_, other.data_ + (rows_ * cols_), data_);
        std::cout << "    [COPY ctor]    " << describe() << " — copied " << rows_ * cols_
                  << " doubles\n";
    }

    // -- Move constructor: steal the pointer — O(1) --
    // No data is copied. We take ownership of other's heap array and
    // leave other in a valid but empty state (data_=nullptr, size=0).
    Matrix(Matrix&& other) noexcept : rows_{other.rows_}, cols_{other.cols_}, data_{other.data_} {
        other.data_ = nullptr;  // leave source valid but empty
        other.rows_ = 0;
        other.cols_ = 0;
        std::cout << "    [MOVE ctor]    " << describe() << " — stole pointer, zero data copied\n";
    }

    // -- Copy assignment --
    Matrix& operator=(const Matrix& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        data_ = new double[static_cast<size_t>(rows_ * cols_)];
        std::copy(other.data_, other.data_ + rows_ * cols_, data_);
        std::cout << "    [COPY assign]  " << describe() << "\n";
        return *this;
    }

    // -- Move assignment --
    Matrix& operator=(Matrix&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] data_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        data_ = other.data_;
        other.data_ = nullptr;
        other.rows_ = 0;
        other.cols_ = 0;
        std::cout << "    [MOVE assign]  " << describe() << "\n";
        return *this;
    }

    // -- Element access --
    inline double& operator()(int r, int c) { return data_[r * cols_ + c]; }
    inline double operator()(int r, int c) const { return data_[r * cols_ + c]; }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    int rows_;
    int cols_;
    double* data_;

    std::string describe() const {
        return "Matrix(" + std::to_string(rows_) + "x" + std::to_string(cols_) + ") @ " +
               (data_ ? std::to_string(reinterpret_cast<uintptr_t>(data_)) : "null");
    }
};

// ===============================================================================
// operator+: adds two matrices element-wise and returns by value.
//
// 'res' is a named local variable. On return the compiler may:
//   a) NRVO:  construct res directly in m3's storage — no move at all
//   b) Move:  move res into m3 — O(1) pointer steal
//   c) Copy:  copy res into m3 — O(rows*cols), only if no move ctor
//
// We do NOT use new/delete here. We let the Matrix handle its own
// memory — RAII means no manual resource management is needed.
// ===============================================================================
Matrix operator+(const Matrix& a, const Matrix& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        throw std::invalid_argument("Matrix dimensions must match for +");
    }

    Matrix res{a.rows(), a.cols()};

    for (int r = 0; r < a.rows(); ++r) {
        for (int c = 0; c < a.cols(); ++c) {
            res(r, c) = a(r, c) + b(r, c);
        }
    }

    return res;  // NRVO may apply; if not, move constructor fires
}

// ===============================================================================
// time_copy: measures the cost of explicitly copying a matrix.
// This is the expensive path — O(rows * cols) data transfer.
// ===============================================================================
void time_copy(const Matrix& m) {
    auto t1 = hires_clock::now();
    Matrix copy{m};  // explicitly invoke copy constructor
    auto t2 = hires_clock::now();
    auto us = std::chrono::duration_cast<micros_t>(t2 - t1).count();
    std::cout << "    copy took: " << us << " us\n";
}

// ===============================================================================
// time_move: measures the cost of moving a matrix.
// This is the cheap path — O(1) pointer swap.
// ===============================================================================
void time_move(Matrix&& m) {
    auto t1 = hires_clock::now();
    Matrix moved{std::move(m)};  // explicitly invoke move constructor
    auto t2 = hires_clock::now();
    auto us = std::chrono::duration_cast<micros_t>(t2 - t1).count();
    std::cout << "    move took: " << us << " us\n";
}

int main() {
    const int N = 1000;  // 1000x1000 = 8 MB of doubles — expensive to copy

    // ----------------------------------------------------------
    // Part 1: operator+() return — the central case from BS
    //
    // With NRVO:  no copy or move at all — res built directly in m3
    // Without:    move constructor fires — cheap pointer steal
    // Either way: no copy of 8 MB of data
    // ----------------------------------------------------------
    std::cout << "\n=========== OPERATOR+ RETURN ===========\n";
    std::cout << "(compile with -fno-elide-constructors to force move)\n\n";
    {
        Matrix m1{N, N};
        Matrix m2{N, N};
        std::cout << "\n  Matrix m3 = m1 + m2:\n";
        Matrix m3 = m1 + m2;  // no copy — move or NRVO
        std::cout << "  m3 ready\n";
    }

    // ----------------------------------------------------------
    // Part 2: explicit copy — the expensive path for contrast
    // ----------------------------------------------------------
    std::cout << "\n=========== EXPLICIT COPY (for contrast) ===========\n";
    {
        Matrix src{N, N};
        std::cout << "\n  Matrix copy = src:   (deep copies " << N * N << " doubles)\n";
        Matrix copy = src;  // copy constructor — O(N*N)
        std::cout << "  copy ready\n";
    }

    // ----------------------------------------------------------
    // Part 3: explicit move — cheap, ownership transferred
    // ----------------------------------------------------------
    std::cout << "\n=========== EXPLICIT MOVE ===========\n";
    {
        Matrix src{N, N};
        std::cout << "\n  Matrix moved = std::move(src):  (steals pointer)\n";
        Matrix moved = std::move(src);  // move constructor — O(1)
        std::cout << "  moved ready — src is now empty\n";
    }

    // ----------------------------------------------------------
    // Part 4: timing — makes the performance argument concrete
    // ----------------------------------------------------------
    std::cout << "\n=========== TIMING: COPY vs MOVE (" << N << "x" << N << " Matrix) ===========\n";
    {
        Matrix m{N, N};

        std::cout << "\n  copying " << N * N << " doubles (~"
                  << (N * N * sizeof(double)) / (1024 * 1024) << " MB):\n";
        time_copy(m);

        std::cout << "\n  moving (pointer steal — size-independent):\n";
        time_move(std::move(m));
    }

    return 0;
}
