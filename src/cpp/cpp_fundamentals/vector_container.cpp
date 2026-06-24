// vector_container.cpp: Faisal Bhuiyan
// Description: A concrete container class illustrating RAII, the Rule of Five,
//              range-checked access, initializer list construction, and push_back().

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

/*
 * A container is an object holding a collection of elements. Vector is the
 * most fundamental container: a variable-size sequence of elements stored
 * contiguously in memory.
 *
 * The constructor/destructor combination is the basis for RAII. The Vector
 * acquires its heap memory in the constructor and releases it in the
 * destructor — automatically, regardless of how the Vector's scope is exited.
 *
 * Because Vector manages a raw resource (a heap array), we must follow the
 * Rule of Five. Whenever a class defines one of these, it should define all:
 *   1. Destructor          — releases the resource
 *   2. Copy constructor    — deep copy:  new allocation, elements copied
 *   3. Copy assignment     — deep copy:  release old, allocate new
 *   4. Move constructor    — steals the pointer: O(1), leaves source empty
 *   5. Move assignment     — same, releasing destination first
 *
 * Without the copy and move operations, the compiler generates memberwise
 * copies — which would copy the raw pointer, leaving two Vectors believing
 * they own the same memory. The destructor would then call delete[] twice:
 * undefined behavior.
 *
 * Vector obeys the same rules for naming, scope, allocation, and lifetime
 * as a built-in type. The established class invariant is:
 *   0 <= sz <= cap   and   elem points to cap doubles (or is nullptr if cap == 0)
 */

// ===============================================================================
// Vector: a concrete container type owning a heap-allocated
//         array of doubles. Fixed-size handle, variable data.
// ===============================================================================
class Vector {
public:
    //-------------------------------------
    //  Constructors
    //-------------------------------------

    // -- Default constructor --
    Vector() : sz{0}, cap{0}, elem{nullptr} { std::cout << "    [ctor]       Vector() default\n"; }

    // -- Size constructor: allocates sz elements, zero-initialised --
    // Invariant check: size must be non-negative
    explicit Vector(int s) : sz{s}, cap{s}, elem{new double[s]{}} {
        if (s < 0) {
            throw std::length_error{"Vector: negative size"};
        }
        std::cout << "    [ctor]       Vector(" << sz << ") size\n";
    }

    // -- Initializer list constructor: Vector v = {1., 2., 3.} --
    Vector(std::initializer_list<double> lst)
        : sz{static_cast<int>(lst.size())},
          cap{static_cast<int>(lst.size())},
          elem{new double[lst.size()]} {
        int i = 0;
        for (double d : lst) {
            elem[i++] = d;
        }
        std::cout << "    [ctor]       Vector{...} initializer_list, sz=" << sz << "\n";
    }

    //-------------------------------------
    //  Rule of Five
    //-------------------------------------

    // -- Destructor: RAII — releases heap memory automatically --
    ~Vector() {
        std::cout << "    [dtor]       Vector sz=" << sz << "\n";
        delete[] elem;
    }

    // -- Copy constructor: deep copy —  O(sz) --
    Vector(const Vector& v) : sz{v.sz}, cap{v.sz}, elem{new double[v.sz]} {
        for (int i = 0; i < sz; ++i) {
            elem[i] = v.elem[i];
        }
        std::cout << "    [copy ctor]  Vector sz=" << sz << " — copied " << sz << " doubles\n";
    }

    // -- Copy assignment: release old memory, deep copy new --
    Vector& operator=(const Vector& v) {
        if (this == &v) {
            return *this;
        }
        double* p = new double[v.sz];
        for (int i = 0; i < v.sz; ++i) {
            p[i] = v.elem[i];
        }
        delete[] elem;
        elem = p;
        sz = v.sz;
        cap = v.sz;
        std::cout << "    [copy assign] Vector sz=" << sz << "\n";
        return *this;
    }

    // -- Move constructor: steal pointer — O(1) --
    Vector(Vector&& v) noexcept : sz{v.sz}, cap{v.cap}, elem{v.elem} {
        v.elem = nullptr;  // leave source in valid but empty state
        v.sz = 0;
        v.cap = 0;
        std::cout << "    [move ctor]  Vector sz=" << sz << " — stole pointer\n";
    }

    // -- Move assignment: release destination, steal source --
    Vector& operator=(Vector&& v) noexcept {
        if (this == &v) {
            return *this;
        }
        delete[] elem;
        elem = v.elem;
        sz = v.sz;
        cap = v.cap;
        v.elem = nullptr;
        v.sz = 0;
        v.cap = 0;
        std::cout << "    [move assign] Vector sz=" << sz << "\n";
        return *this;
    }

    // -- Unchecked element access: operator[] --
    // Fast, no bounds check — caller is responsible for valid index.
    inline double& operator[](int i) { return elem[i]; }
    inline double operator[](int i) const { return elem[i]; }

    // -- Range-checked element access: at() --
    // Throws std::out_of_range if i is outside [0, sz).
    // Provides safety where correctness matters more than raw speed.
    double& at(int i) {
        if (i < 0 || i >= sz) {
            throw std::out_of_range{
                "Vector::at(): index " + std::to_string(i) + " out of range [0, " +
                std::to_string(sz) + ")"
            };
        }
        return elem[i];
    }

    double at(int i) const {
        if (i < 0 || i >= sz) {
            throw std::out_of_range{
                "Vector::at(): index " + std::to_string(i) + " out of range [0, " +
                std::to_string(sz) + ")"
            };
        }
        return elem[i];
    }

    // -- push_back: append one element, growing capacity if needed --
    // Capacity doubles each time — amortised O(1) per push_back.
    void push_back(double d) {
        if (sz == cap) {
            reserve(cap == 0 ? 1 : 2 * cap);
        }
        elem[sz++] = d;
    }

    // -- reserve: ensure capacity for at least newcap elements --
    // Does not change sz. Useful to avoid repeated reallocations.
    void reserve(int newcap) {
        if (newcap <= cap)
            return;
        double* p = new double[newcap];
        for (int i = 0; i < sz; ++i)
            p[i] = elem[i];
        delete[] elem;
        elem = p;
        cap = newcap;
        std::cout << "    [reserve]    capacity: " << cap << " -> " << newcap << "\n";
    }

    // -- Iteration support: allows range-for and standard algorithms --
    double* begin() { return elem; }
    double* end() { return elem + sz; }
    const double* begin() const { return elem; }
    const double* end() const { return elem + sz; }

    // -- Capacity queries --
    inline int size() const { return sz; }
    inline int capacity() const { return cap; }
    inline bool empty() const { return sz == 0; }

private:
    int sz;        // number of elements currently stored
    int cap;       // number of doubles allocated (cap >= sz)
    double* elem;  // pointer to cap doubles on the free store
};

/*
 * The Vector object is a fixed-size "handle" on the stack containing a
 * pointer to a variable-sized array on the heap. The handle is always
 * sizeof(Vector) bytes — two ints and a pointer — regardless of how many
 * elements are stored. This separation of handle and data is the fundamental
 * technique for managing variable amounts of information in C++.
 */

// ===============================================================================
// read_and_sum: reads s values into a Vector via range-checked
// at(), demonstrating that out-of-range access is caught cleanly.
// ===============================================================================
double read_and_sum(int s) {
    Vector v(s);

    std::cout << "  provide " << s << " values: ";
    for (int i = 0; i != v.size(); ++i) {
        std::cin >> v.at(i);  // range-checked: throws if i is invalid
    }

    double sum = 0;
    for (double d : v) {  // range-for via begin()/end()
        sum += d;
    }
    return sum;
}

int main() {
    // ----------------------------------------------------------
    // Construction — three ways to build a Vector
    // ----------------------------------------------------------
    std::cout << "\n=========== CONSTRUCTION ===========\n";
    {
        Vector v1;                  // default: empty
        Vector v2(4);               // size: 4 zero-initialised doubles
        Vector v3{1., 2., 3., 4.};  // initializer list

        std::cout << "  v1.size()=" << v1.size() << "  v2.size()=" << v2.size()
                  << "  v3.size()=" << v3.size() << "\n";
        std::cout << "  v3 elements: ";
        for (double d : v3) {
            std::cout << d << " ";
        }
        std::cout << "\n";
    }

    // ----------------------------------------------------------
    // push_back and capacity growth
    // ----------------------------------------------------------
    std::cout << "\n=========== PUSH_BACK AND CAPACITY GROWTH ===========\n";
    {
        Vector v;
        std::cout << "  pushing 5 elements:\n";
        for (int i = 1; i <= 5; ++i) {
            v.push_back(i * 1.5);
            std::cout << "    sz=" << v.size() << "  cap=" << v.capacity() << "\n";
        }
        std::cout << "  elements: ";
        for (double d : v) {
            std::cout << d << " ";
        }
        std::cout << "\n";
    }

    // ----------------------------------------------------------
    // Range-checked access: at() vs operator[]
    // ----------------------------------------------------------
    std::cout << "\n=========== RANGE-CHECKED ACCESS ===========\n";
    {
        Vector v{10., 20., 30.};

        std::cout << "  v[1]    = " << v[1] << "  (unchecked)\n";
        std::cout << "  v.at(1) = " << v.at(1) << "  (range-checked)\n";

        try {
            [[maybe_unused]] double d = v.at(10);  // out of range
        } catch (const std::out_of_range& e) {
            std::cout << "  caught: " << e.what() << "\n";
        }
    }

    // ----------------------------------------------------------
    // Copy and move — Rule of Five in action
    // ----------------------------------------------------------
    std::cout << "\n=========== COPY AND MOVE ===========\n";
    {
        Vector original{1., 2., 3.};

        std::cout << "\n  copy construction:\n";
        Vector copy = original;  // copy constructor
        copy[0] = 99.;           // modifying copy does not affect original
        std::cout << "  original[0]=" << original[0] << "  copy[0]=" << copy[0]
                  << "  (independent)\n";

        std::cout << "\n  move construction:\n";
        Vector moved = std::move(original);  // move constructor
        std::cout << "  moved.size()=" << moved.size() << "  original.size()=" << original.size()
                  << "  (source emptied)\n";
    }

    // ----------------------------------------------------------
    // RAII: exception safety — destructor fires even if we throw
    // ----------------------------------------------------------
    std::cout << "\n=========== RAII: EXCEPTION SAFETY ===========\n";
    {
        try {
            Vector v{5., 10., 15.};
            std::cout << "  Vector alive, about to throw\n";
            throw std::runtime_error("simulated error");
        } catch (const std::runtime_error& e) {
            std::cout << "  caught: " << e.what() << " — Vector already destroyed above\n";
        }
    }

    // ----------------------------------------------------------
    // read_and_sum: interactive use
    // ----------------------------------------------------------
    std::cout << "\n=========== READ AND SUM ===========\n";
    int n = 3;
    double sum = read_and_sum(n);
    std::cout << "  sum of " << n << " elements = " << sum << "\n";

    return 0;
}
