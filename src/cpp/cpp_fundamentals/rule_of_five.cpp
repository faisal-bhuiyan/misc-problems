// vector_rule_of_five.cpp: Faisal Bhuiyan
// Description: Rule of Five for a resource-owning Vector.
//              The matched set, shallow copy disaster, std::move as a cast,
//              copy elision vs move assignment, and when to use std::move.

#include <initializer_list>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

/*
 * CTORS, DTORS, COPY AND MOVE — A MATCHED SET
 *
 * For a class that owns a resource through a raw pointer, the five essential
 * operations are logically inseparable:
 *
 *   Destructor        — releases the resource
 *   Copy constructor  — acquires a new resource and copies the state
 *   Copy assignment   — releases old resource, acquires new one, copies state
 *   Move constructor  — steals the resource, leaves source empty
 *   Move assignment   — releases own resource, steals source's, empties source
 *
 * Define any one and you imply all five. The compiler's defaults
 * for the missing ones are almost always wrong for a resource handle:
 *   - Default copy: memberwise copy of the raw pointer — shallow copy
 *     Two objects believe they own the same memory → double-free on destruction
 *   - Default move (if copy defined): not generated — falls back to copy
 *     Every move becomes an expensive O(n) copy instead of O(1) pointer steal
 *
 * STD::MOVE IS JUST A CAST
 *
 * std::move(x) does NOT move anything. It casts x to an rvalue reference:
 *   static_cast<std::remove_reference_t<T>&&>(x)
 * The actual transfer of resources happens in the move constructor or
 * move assignment — not in std::move. A named variable passed to std::move
 * retains its value until a move constructor or move assignment uses it.
 *
 * COPY ELISION vs MOVE ASSIGNMENT
 *
 * The compiler is obliged to eliminate most copies associated with
 * initialisation (RVO/NRVO). The result is even better than a move:
 * the object is constructed directly in its final storage — zero overhead.
 *
 * However, elision applies only to initialisation, not assignment.
 * In:   v = make_vector();
 * the compiler cannot elide — make_vector() builds into a temporary, then
 * the move assignment transfers it into v. Move assignment is therefore
 * critical for performance in assignment contexts.
 */

// ====================================================================
// DangerousVector: defines ONLY a destructor.
// The compiler silently generates a copy constructor that copies
// the raw pointer — two objects then share the same heap memory.
// ====================================================================
class DangerousVector {
public:
    explicit DangerousVector(int sz) : data_{new double[sz]{}}, sz_{sz} {
        std::cout << "    [ctor] DangerousVector  data_@" << data_ << "\n";
    }

    ~DangerousVector() {
        std::cout << "    [dtor] DangerousVector  data_@" << data_ << "\n";
        delete[] data_;
    }

    // The compiler generates copy as: dv2.data_ = dv1.data_  (copies pointer!)
    // Both objects believe they own the same memory — invariant violated.

    const double* raw() const { return data_; }
    int size() const { return sz_; }

private:
    double* data_;
    int sz_;
};

// ====================================================================
// Vector: complete Rule of Five — all five operations defined
//         as a matched set. Each is traced to show when it fires.
// ====================================================================
class Vector {
public:
    // -- Default constructor --
    Vector() : data_{nullptr}, sz_{0}, cap_{0} { std::cout << "    [default ctor]   Vector()\n"; }

    // -- Size constructor --
    explicit Vector(int sz) : data_{new double[sz]{}}, sz_{sz}, cap_{sz} {
        std::cout << "    [size ctor]      Vector(" << sz_ << ")\n";
    }

    // -- Initializer list constructor --
    Vector(std::initializer_list<double> lst)
        : data_{new double[lst.size()]},
          sz_{static_cast<int>(lst.size())},
          cap_{static_cast<int>(lst.size())} {
        int i = 0;
        for (double d : lst) {
            this->data_[i++] = d;
        }
        std::cout << "    [list ctor]      Vector{";
        for (int j = 0; j < this->sz_; ++j) {
            std::cout << this->data_[j] << (j < this->sz_ - 1 ? ", " : "");
        }
        std::cout << "}\n";
    }

    // 1. Destructor
    ~Vector() {
        std::cout << "    [dtor]           Vector sz=" << this->sz_ << " data_@" << this->data_
                  << "\n";
        delete[] this->data_;
    }

    // 2. Copy constructor — deep copy: allocate new memory, copy all elements
    Vector(const Vector& v) : data_{new double[v.sz_]}, sz_{v.sz_}, cap_{v.sz_} {
        // Copy data from v to this
        for (int i = 0; i < this->sz_; ++i) {
            this->data_[i] = v.data_[i];
        }
        std::cout << "    [copy ctor]      Vector sz=" << this->sz_ << " — deep copied " << this->sz_
                  << " doubles\n";
    }

    // 3. Copy assignment — release old memory, deep copy new
    Vector& operator=(const Vector& v) {
        if (this == &v) {
            return *this;
        }

        // Release the old resource
        delete[] this->data_;

        // Allocate the new resource
        this->data_ = new double[v.sz_];

        // Copy the data
        this->sz_ = v.sz_;
        this->cap_ = v.sz_;
        for (int i = 0; i < this->sz_; ++i) {
            this->data_[i] = v.data_[i];
        }
        std::cout << "    [copy assign]    Vector sz=" << this->sz_ << " — deep copied " << this->sz_
                  << " doubles\n";
        return *this;
    }

    // 4. Move constructor — steal pointer: O(1) regardless of size
    Vector(Vector&& v) noexcept : data_{v.data_}, sz_{v.sz_}, cap_{v.cap_} {
        // Leave the source in a valid but empty state
        v.data_ = nullptr;
        v.sz_ = 0;
        v.cap_ = 0;
        std::cout << "    [move ctor]      Vector sz=" << this->sz_
                  << " — stole pointer, zero data copied\n";
    }

    // 5. Move assignment — release own resource, steal source's
    Vector& operator=(Vector&& v) noexcept {
        // Check for self-assignment
        if (this == &v) {
            return *this;
        }

        // Release the old resource
        delete[] this->data_;

        // Transfer ownership of the resource
        this->data_ = v.data_;
        this->sz_ = v.sz_;
        this->cap_ = v.cap_;

        // Leave the source in a valid but empty state
        v.data_ = nullptr;
        v.sz_ = 0;
        v.cap_ = 0;
        std::cout << "    [move assign]    Vector sz=" << this->sz_
                  << " — stole pointer, zero data copied\n";
        return *this;
    }

    void push_back(double d) {
        if (this->sz_ == this->cap_)
            reserve(this->cap_ == 0 ? 1 : 2 * this->cap_);
        this->data_[this->sz_++] = d;
    }

    void reserve(int newcap) {
        if (newcap <= this->cap_) {
            return;
        }
        // Allocate the new resource
        double* p = new double[newcap];
        // Copy the data
        for (int i = 0; i < this->sz_; ++i) {
            p[i] = this->data_[i];
        }
        // Release the old resource
        delete[] this->data_;
        // Transfer ownership of the resource
        this->data_ = p;
        this->cap_ = newcap;
    }

    double& operator[](int i) { return this->data_[i]; }
    double operator[](int i) const { return this->data_[i]; }
    int size() const { return this->sz_; }
    int capacity() const { return this->cap_; }

    double* begin() { return this->data_; }
    double* end() { return this->data_ + this->sz_; }
    const double* begin() const { return this->data_; }
    const double* end() const { return this->data_ + this->sz_; }

    void print(const std::string& label) const {
        std::cout << "    " << label << ": sz=" << this->sz_ << " cap=" << this->cap_ << " data_@"
                  << (void*)this->data_ << "  [";
        for (int i = 0; i < this->sz_; ++i) {
            std::cout << this->data_[i] << (i < this->sz_ - 1 ? ", " : "");
        }
        std::cout << "]\n";
    }

private:
    double* data_;
    int sz_;
    int cap_;
};

// ====================================================================
// my_move: a transparent re-implementation of std::move.
//
// std::move does exactly this — casts to an rvalue reference.
// The name "move" is misleading: it does not move anything.
// It changes the type of the expression so that overload
// resolution selects the move constructor or move assignment
// rather than the copy constructor or copy assignment.
// ====================================================================
template <typename T>
constexpr std::remove_reference_t<T>&& my_move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}

// ====================================================================
// make_vector(): factory used to demonstrate copy elision and
//               move assignment.
// ====================================================================
Vector make_vector() {
    Vector v{1., 2., 3., 4., 5.};
    return v;  // NRVO: compiler may build v directly in caller's storage
               // With -fno-elide-constructors: move ctor fires instead
}

// ====================================================================
// Pipeline: demonstrates explicit std::move in a constructor —
// a class member cannot be NRVO-elided into; std::move is required.
// ====================================================================
class Pipeline {
public:
    explicit Pipeline(Vector input)  // sink parameter — takes by value
        : data_{std::move(input)}    // ✅ must move: input is a named lvalue
    {
        std::cout << "    [Pipeline ctor]  data_ sz=" << data_.size() << "\n";
    }

    Vector extract() {
        return std::move(data_);  // ✅ must move: data_ is a member, not a local
    }  //    NRVO does not apply to members

private:
    Vector data_;
};

int main() {
    // ----------------------------------------------------------
    // Part 1: Shallow copy disaster — default memberwise copy
    //         of a raw pointer violates the resource invariant
    // ----------------------------------------------------------
    std::cout << "\n=========== SHALLOW COPY DISASTER ===========\n\n";
    {
        DangerousVector dv1{4};
        // DangerousVector dv2 = dv1;  // intentionally not executed
        // Running this would alias dv1.data_ into dv2.data_
        // Both destructors would fire delete[] on the same address.
        // ASAN output: "attempting double-free" — heap corruption.

        std::cout << "  dv1 owns heap @ " << (void*)dv1.raw() << "\n";
        std::cout << "  compiler copy: dv2.data_ = same address — invariant violated\n";
    }

    // ----------------------------------------------------------
    // Part 2: Rule of Five — matched set, correct behaviour
    // ----------------------------------------------------------
    std::cout << "\n=========== RULE OF FIVE — MATCHED SET ===========\n";

    std::cout << "\n  copy — deep, independent:\n";
    {
        Vector v1{10., 20., 30.};
        Vector v2 = v1;  // copy ctor: new allocation, all elements copied
        v2[0] = 99.;     // modifying v2 must not affect v1
        v1.print("v1");
        v2.print("v2");
        std::cout << "  v1[0]=" << v1[0] << "  v2[0]=" << v2[0]
                  << "  (independent — different pointers)\n";
    }

    std::cout << "\n  move — O(1) pointer steal:\n";
    {
        Vector v1{10., 20., 30.};
        v1.print("before move: v1");
        Vector v2 = std::move(v1);  // move ctor: pointer stolen, v1 emptied
        v1.print("after  move: v1");
        v2.print("after  move: v2");
    }

    // ----------------------------------------------------------
    // Part 3: std::move is just a cast — the move happens in the
    //         constructor, not in the std::move call itself
    // ----------------------------------------------------------
    std::cout << "\n=========== STD::MOVE IS JUST A CAST ===========\n\n";
    {
        Vector v1{1., 2., 3.};
        v1.print("v1 before std::move");

        // std::move casts v1 to Vector&& — v1 is UNCHANGED by this call
        auto&& rref = std::move(v1);
        v1.print("v1 AFTER std::move  (unchanged — no move happened yet)");

        // The MOVE CONSTRUCTOR fires here — THIS is what actually moves the data
        Vector v2{std::move(rref)};
        v1.print("v1 after move ctor  (now empty — data was stolen)");
        v2.print("v2 after move ctor  (now owns the data)");

        // my_move is identical to std::move — just a cast
        Vector v3{1., 2.};
        Vector v4 = my_move(v3);  // same result as std::move
        std::cout << "  my_move == std::move: "
                  << (v3.size() == 0 && v4.size() == 2 ? "confirmed" : "wrong") << "\n";
    }

    // ----------------------------------------------------------
    // Part 4: Copy elision vs move assignment
    //
    // Initialisation:  Vector v = make_vector()
    //   → NRVO may apply: v built directly inside make_vector()
    //   → Even better than a move: zero overhead
    //
    // Assignment:      v = make_vector()
    //   → Elision cannot help: make_vector() builds a temporary,
    //     then move assignment transfers it into v
    //   → Move assignment is critical here — without it, copy fires
    // ----------------------------------------------------------
    std::cout << "\n=========== COPY ELISION vs MOVE ASSIGNMENT ===========\n";

    std::cout << "\n  initialisation — NRVO may eliminate even the move:\n";
    {
        Vector v = make_vector();  // with NRVO: [list ctor] only — zero move/copy
        v.print("v after init");   // with -fno-elide-constructors: [move ctor] visible
    }

    std::cout << "\n  assignment — elision cannot apply, move assignment fires:\n";
    {
        Vector v;                   // default ctor
        v = make_vector();          // make_vector() → temp → [move assign] into v
        v.print("v after assign");  // move assignment is what saves us here
    }

    // ----------------------------------------------------------
    // Part 5: When to use explicit std::move
    // ----------------------------------------------------------
    std::cout << "\n=========== WHEN TO USE EXPLICIT STD::MOVE ===========\n";

    // ✅ Case A: push_back — move a local you no longer need
    std::cout << "\n  A) push_back — move local into container:\n";
    {
        std::vector<Vector> store;
        Vector local{5., 6., 7.};
        store.push_back(std::move(local));  // ✅ move: local not needed after this
        std::cout << "  local.size()=" << local.size() << " (emptied)\n";
        std::cout << "  store[0].size()=" << store[0].size() << "\n";
    }

    // ✅ Case B: sink parameter — move from function argument into member
    std::cout << "\n  B) sink parameter — move into class member:\n";
    {
        Vector input{1., 2., 3.};
        Pipeline p{std::move(input)};  // ✅ move: input transferred to Pipeline
        std::cout << "  input.size()=" << input.size() << " (emptied)\n";
    }

    // ✅ Case C: extract member — NRVO does not apply to members
    std::cout << "\n  C) extract member — return std::move(member_):\n";
    {
        Pipeline p{Vector{10., 20., 30.}};
        Vector extracted = p.extract();  // ✅ std::move inside extract() needed
        extracted.print("extracted");
    }

    // ❌ Case D: WRONG — std::move on return of local defeats NRVO
    std::cout << "\n  D) WRONG: std::move on return of local (defeats NRVO):\n";
    {
        // This would PREVENT copy elision — the compiler cannot apply NRVO
        // when std::move is present, so a move fires where zero cost was possible.
        // Correct version is: return v;  (no std::move)
        // The compiler warns: "moving a local object in a return statement prevents copy elision"
        // This warning is exactly right — shown here to confirm the compiler catches it.
        auto bad_return = []() -> Vector {
            Vector v{1., 2.};
            return std::move(v);  // ❌ forces move — elision prevented
        };
        auto good_return = []() -> Vector {
            Vector v{1., 2.};
            return v;  // ✅ NRVO applies — zero overhead
        };
        std::cout << "  bad_return (std::move on local):\n";
        Vector b = bad_return();
        std::cout << "  good_return (no std::move):\n";
        Vector g = good_return();
    }

    // ❌ Case E: WRONG — std::move from const silently copies
    std::cout << "\n  E) WRONG: std::move from const silently copies:\n";
    {
        const Vector cv{1., 2., 3.};
        Vector v = std::move(cv);  // ❌ const T&& cannot bind to move ctor (T&&)
                                   //    silently falls back to copy ctor
        std::cout << "  cv.size()=" << cv.size() << " (unchanged — copy fired, not move)\n";
    }

    return 0;
}
