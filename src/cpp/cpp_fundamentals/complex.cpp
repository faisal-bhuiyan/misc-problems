// complex.cpp: Faisal Bhuiyan
// Description: A concrete arithmetic type that mimics a built-in type.
//              Implements a simplified version of std::complex<double>
//              following Stroustrup's design principles for concrete classes.

#include <cmath>
#include <iostream>
#include <vector>

/*
 * The basic idea of a concrete class is that it behaves just like a built-in
 * type. The defining characteristic is that its representation is part of its
 * definition — re and im are visible right here in the class body.
 *
 * This has important consequences:
 *   - Objects can be allocated on the stack              (no heap required)
 *   - Objects can be embedded directly in other objects  (no indirection)
 *   - Size is fixed and known at compile time            (sizeof(complex) = 16)
 *   - Objects can be placed in arrays and vectors        (contiguous memory)
 *
 * The class definition contains only operations that require direct access
 * to the representation (re, im). Operations that do not require such access
 * — binary +, -, *, /, abs(), conj(), operator<< — are defined separately.
 * This keeps the class minimal and the interface clean.
 *
 * The compound assignment operators (+=, -=, *=, /=) are the natural members:
 * they modify the object in place. The binary operators are then trivially
 * derived from them outside the class.
 */

// ===============================================================================
// complex: a concrete arithmetic type representing a + bi
//          where a is the real part and b is the imaginary part.
// ===============================================================================
class complex {
public:
    //-------------------------------------
    //  Constructors
    //-------------------------------------
    // Default: 0 + 0i
    complex() : re_{0.}, im_{0.} {}

    // Real and imaginary parts supplied
    complex(double re, double im) : re_{re}, im_{im} {}

    // Real only — imaginary defaults to 0: models a real number as complex
    complex(double re) : re_{re}, im_{0.} {}

    //-------------------------------------
    //  Getters
    //-------------------------------------
    inline double real() const { return re_; }
    inline double imag() const { return im_; }

    //-------------------------------------
    //  Setters
    //-------------------------------------
    inline void real(double r) { re_ = r; }
    inline void imag(double i) { im_ = i; }

    //-------------------------------------
    //  Compound assignment operators
    //-------------------------------------
    // These require access to re and im, so they live inside the class.
    // The binary operators (+, -, *, /) are built from these outside.

    complex& operator+=(const complex& c) {
        this->re_ += c.re_;
        this->im_ += c.im_;
        return *this;
    }

    complex& operator-=(const complex& c) {
        this->re_ -= c.re_;
        this->im_ -= c.im_;
        return *this;
    }

    // (a + bi)(c + di) = (ac - bd) + (ad + bc)i
    // Must use temporaries: re is needed to compute new im, so we
    // cannot overwrite re before finishing the im calculation.
    complex& operator*=(const complex& c) {
        double r = this->re_ * c.re_ - this->im_ * c.im_;
        double i = this->re_ * c.im_ + this->im_ * c.re_;
        this->re_ = r;
        this->im_ = i;
        return *this;
    }

    // (a + bi)/(c + di) = ((ac + bd) + (bc - ad)i) / (c² + d²)
    complex& operator/=(const complex& c) {
        double denom = c.re_ * c.re_ + c.im_ * c.im_;
        double r = (this->re_ * c.re_ + this->im_ * c.im_) / denom;
        double i = (this->im_ * c.re_ - this->re_ * c.im_) / denom;
        this->re_ = r;
        this->im_ = i;
        return *this;
    }

    //-------------------------------------
    //  Equality
    //-------------------------------------
    inline bool operator==(const complex& c) const {
        return this->re_ == c.re_ && this->im_ == c.im_;
    }

    inline bool operator!=(const complex& c) const { return !(*this == c); }

private:
    double re_;  // real part
    double im_;  // imaginary part
};

// ===============================================================================
// Operations that do NOT require direct access to re and im.
// They are defined in terms of the compound assignment operators
// and getters — entirely outside the class definition.
// This is the key design point: the class stays minimal.
// ===============================================================================

//-------------------------------------
//  Binary arithmetic operators
//-------------------------------------
// Each takes its left operand by value (a copy) and applies
// the compound operator, then returns the modified copy.

inline complex operator+(complex a, const complex& b) {
    return a += b;
}
inline complex operator-(complex a, const complex& b) {
    return a -= b;
}
inline complex operator*(complex a, const complex& b) {
    return a *= b;
}
inline complex operator/(complex a, const complex& b) {
    return a /= b;
}

// -- Unary negation --
inline complex operator-(const complex& a) {
    return {-a.real(), -a.imag()};
}

// -- Magnitude: |a + bi| = sqrt(a² + b²) --
inline double abs(const complex& c) {
    return std::sqrt(c.real() * c.real() + c.imag() * c.imag());
}

// -- Complex conjugate: conj(a + bi) = a - bi --
inline complex conj(const complex& c) {
    return {c.real(), -c.imag()};
}

// -- Output: prints in the form (a, b) --
std::ostream& operator<<(std::ostream& os, const complex& c) {
    return os << "(" << c.real() << ", " << c.imag() << ")";
}

// ===============================================================================
// Demonstrate that complex behaves like a built-in type:
//   - local variables on the stack
//   - embedded in another class
//   - stored in a vector (contiguous memory, no indirection)
//   - passed and returned by value
// ===============================================================================
struct Impedance {
    // complex embedded directly — no pointer, no heap
    complex resistance;
    complex reactance;

    Impedance(complex r, complex x) : resistance{r}, reactance{x} {}

    complex total() const { return resistance + reactance; }
};

int main() {
    // ----------------------------------------------------------
    // Basic construction — on the stack, like double or int
    // ----------------------------------------------------------
    std::cout << "\n=========== CONSTRUCTION ===========\n";

    complex c1{3., 4.};  // 3 + 4i
    complex c2{1., 2.};  // 1 + 2i
    complex c3{5.};      // 5 + 0i  (real-only constructor)
    complex c4{};        // 0 + 0i  (default)

    std::cout << "  c1 = " << c1 << "\n";
    std::cout << "  c2 = " << c2 << "\n";
    std::cout << "  c3 = " << c3 << "  (real-only constructor)\n";
    std::cout << "  c4 = " << c4 << "  (default constructor)\n";

    // ----------------------------------------------------------
    // Arithmetic — exactly like built-in numeric types
    // ----------------------------------------------------------
    std::cout << "\n=========== ARITHMETIC ===========\n";

    std::cout << "  c1 + c2 = " << c1 + c2 << "\n";
    std::cout << "  c1 - c2 = " << c1 - c2 << "\n";
    std::cout << "  c1 * c2 = " << c1 * c2 << "\n";
    std::cout << "  c1 / c2 = " << c1 / c2 << "\n";
    std::cout << "  -c1     = " << -c1 << "\n";

    // ----------------------------------------------------------
    // Compound assignment — modifies in place, like int +=
    // ----------------------------------------------------------
    std::cout << "\n=========== COMPOUND ASSIGNMENT ===========\n";

    complex acc{1., 0.};
    std::cout << "  acc         = " << acc << "\n";
    acc += c1;
    std::cout << "  acc += c1   = " << acc << "\n";
    acc *= c2;
    std::cout << "  acc *= c2   = " << acc << "\n";

    // ----------------------------------------------------------
    // Magnitude and conjugate — free functions outside the class
    // ----------------------------------------------------------
    std::cout << "\n=========== FREE FUNCTIONS ===========\n";

    std::cout << "  abs(c1)    = " << abs(c1) << "  (|3 + 4i| = 5)\n";
    std::cout << "  conj(c1)   = " << conj(c1) << "  (conjugate of 3 + 4i)\n";

    // ----------------------------------------------------------
    // Equality
    // ----------------------------------------------------------
    std::cout << "\n=========== EQUALITY ===========\n";

    complex a{1., 2.};
    complex b{1., 2.};
    complex c{1., 3.};

    std::cout << "  (1,2) == (1,2): " << (a == b ? "true" : "false") << "\n";
    std::cout << "  (1,2) != (1,3): " << (a != c ? "true" : "false") << "\n";

    // ----------------------------------------------------------
    // In a vector — contiguous in memory, no indirection
    // Concrete types work naturally with standard containers
    // ----------------------------------------------------------
    std::cout << "\n=========== IN A VECTOR ===========\n";

    std::vector<complex> roots{{1., 0.}, {-1., 0.}, {0., 1.}, {0., -1.}};

    std::cout << "  fourth roots of unity:\n";
    for (const auto& r : roots) {
        std::cout << "    " << r << "  |abs| = " << abs(r) << "\n";
    }

    // ----------------------------------------------------------
    // Embedded in another class — no heap, no pointer
    // ----------------------------------------------------------
    std::cout << "\n=========== EMBEDDED IN ANOTHER CLASS ===========\n";

    Impedance z{complex{50., 0.}, complex{0., 30.}};
    std::cout << "  resistance = " << z.resistance << "\n";
    std::cout << "  reactance  = " << z.reactance << "\n";
    std::cout << "  total      = " << z.total() << "\n";

    // ----------------------------------------------------------
    // Getters and setters — like a built-in type's assignment
    // ----------------------------------------------------------
    std::cout << "\n=========== GETTERS AND SETTERS ===========\n";

    complex q{2., 3.};
    std::cout << "  q        = " << q << "\n";
    std::cout << "  q.real() = " << q.real() << "\n";
    std::cout << "  q.imag() = " << q.imag() << "\n";
    q.real(10.);
    q.imag(20.);
    std::cout << "  q after real(10), imag(20): " << q << "\n";

    return 0;
}
