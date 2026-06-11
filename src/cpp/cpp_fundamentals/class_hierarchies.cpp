// class_hierarchies.cpp: Faisal Bhuiyan
// Description: A class hierarchy for shapes. Demonstrates virtual destructors
//              in ABCs, the Stroustrup read_shape()/user() factory pattern,
//              hierarchy navigation via dynamic_cast, and smart pointer RAII.

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

/*
 * A virtual destructor is essential for any ABC through whose interface
 * derived objects will be deleted. Without it, deleting through Shape*
 * calls only ~Shape() — any resources held by the derived class (the
 * Smiley's eyes_ and mouth_ unique_ptrs, for example) are silently leaked.
 *
 * The BS class hierarchy demonstrated here:
 *   Shape      — ABC, pure virtual interface, virtual destructor
 *   Circle     — concrete Shape: center + radius
 *   Triangle   — concrete Shape: three vertices, shoelace area
 *   Rectangle  — concrete Shape: center + width + height
 *   Smiley     — multi-level: Smiley -> Circle -> Shape
 *                owns eye and mouth Shapes via unique_ptr
 *
 * Three additional design points:
 *
 * 1. read_shape() factory: creates a concrete Shape from a stream,
 *    returns ownership as unique_ptr<Shape>. The caller never sees
 *    the concrete type — it receives a base-class handle.
 *
 * 2. user(): builds a vector<unique_ptr<Shape>>, applies operations.
 *    No manual delete — unique_ptr guarantees cleanup even if an
 *    exception is thrown anywhere in the function.
 *
 * 3. dynamic_cast: navigate the hierarchy when a derived-class feature
 *    is genuinely needed that does not belong in the base interface.
 *    Use with restraint — see the section below for the full rationale.
 */

// ===============================================================================
// Point: 2D Cartesian coordinate.
// Concrete type — two doubles, stack-allocatable, no heap.
// ===============================================================================
class Point {
public:
    Point(double x = 0., double y = 0.) : x_{x}, y_{y} {}

    double GetX() const { return this->x_; }
    double GetY() const { return this->y_; }
    void SetX(double v) { this->x_ = v; }
    void SetY(double v) { this->y_ = v; }

    double Distance(const Point& p) const {
        return std::sqrt(
            (this->x_ - p.x_) * (this->x_ - p.x_) + (this->y_ - p.y_) * (this->y_ - p.y_)
        );
    }

private:
    double x_;
    double y_;
};

inline Point operator+(const Point& a, const Point& b) {
    return {a.GetX() + b.GetX(), a.GetY() + b.GetY()};
}

inline bool operator==(const Point& a, const Point& b) {
    return std::abs(a.GetX() - b.GetX()) < 1e-9 && std::abs(a.GetY() - b.GetY()) < 1e-9;
}

inline bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
}

// ===============================================================================
// Shape: abstract base class (ABC)
//
// Contains only pure virtual methods — no data and no constructor.
// virtual ~Shape() is essential: without it, delete through Shape*
// invokes only ~Shape(), bypassing derived class cleanup entirely.
// = default is sufficient here since Shape itself holds no resources.
// ===============================================================================
class Shape {
public:
    virtual Point center() const = 0;
    virtual double area() const = 0;
    virtual void move(Point to) = 0;
    virtual void draw() const = 0;
    virtual void rotate(int angle) = 0;

    virtual ~Shape() = default;  // ESSENTIAL —> enables correct derived cleanup
};

// ===============================================================================
// Circle: concrete Shape.
// kPI is static constexpr — belongs to the class, not each object.
// radius() is NOT in Shape — it is Circle-specific (Triangle has no radius).
// This is why dynamic_cast is sometimes justified for radius access.
// ===============================================================================
class Circle : public Shape {
public:
    static constexpr double kPI{3.14159265358979};

    Circle(Point center, double radius) : center_{center}, radius_{radius} {}

    Point center() const override { return center_; }
    double radius() const { return radius_; }  // Circle-specific — not in Shape
    double area() const override { return kPI * radius_ * radius_; }
    void move(Point to) override { center_ = to; }

    void draw() const override {
        std::cout << "    Circle    at (" << center_.GetX() << ", " << center_.GetY()
                  << ")  r=" << radius_ << "  area=" << area() << "\n";
    }

    void rotate(int angle) override {
        std::cout << "    Circle    is symmetric — " << angle << "deg: no change\n";
    }

    ~Circle() override = default;

protected:
    Point center_;
    double radius_;
};

// ===============================================================================
// Triangle: concrete Shape defined by three vertices.
// Area via the shoelace formula. Center is the centroid.
// ===============================================================================
class Triangle : public Shape {
public:
    Triangle(Point a, Point b, Point c) : a_{a}, b_{b}, c_{c} {
        center_ = Point{
            (a_.GetX() + b_.GetX() + c_.GetX()) / 3.,  // x
            (a_.GetY() + b_.GetY() + c_.GetY()) / 3.   // y
        };
    }

    Point center() const override { return center_; }

    double area() const override {
        // Shoelace formula: |((b-a) × (c-a))| / 2
        return 0.5 * std::abs(
                         (b_.GetX() - a_.GetX()) * (c_.GetY() - a_.GetY()) -
                         (c_.GetX() - a_.GetX()) * (b_.GetY() - a_.GetY())
                     );
    }

    void move(Point to) override {
        const double dx = to.GetX() - center_.GetX();
        const double dy = to.GetY() - center_.GetY();
        const Point delta{dx, dy};
        a_ = a_ + delta;
        b_ = b_ + delta;
        c_ = c_ + delta;
        center_ = to;
    }

    void draw() const override {
        std::cout << "    Triangle  at (" << center_.GetX() << ", " << center_.GetY()
                  << ")  area=" << area() << "\n";
    }

    void rotate(int angle) override {
        std::cout << "    Triangle  rotated " << angle << " degrees\n";
    }

private:
    Point a_;
    Point b_;
    Point c_;
    Point center_;
};

// ===============================================================================
// Rectangle: concrete Shape.
// Kept from the original — inherits all Shape interface obligations.
// ===============================================================================
class Rectangle : public Shape {
public:
    Rectangle(double width, double height, double cx = 0., double cy = 0.)
        : center_{cx, cy}, width_{width}, height_{height} {}

    Point center() const override { return center_; }
    double area() const override { return width_ * height_; }
    void move(Point to) override { center_ = to; }

    void draw() const override {
        std::cout << "    Rectangle at (" << center_.GetX() << ", " << center_.GetY() << ")  "
                  << width_ << "x" << height_ << "  area=" << area() << "\n";
    }

    void rotate(int angle) override {
        std::cout << "    Rectangle rotated " << angle << " degrees\n";
    }

protected:
    Point center_;
    double width_;
    double height_;
};

// ===============================================================================
// Smiley: multi-level inheritance — Smiley IS a Circle.
//
// Owns eyes_ and mouth_ as unique_ptr<Shape>.
// RAII ensures they are freed when Smiley is destroyed.
// This only works correctly because ~Shape() is virtual:
// when delete fires on a Shape* pointing to a Smiley, the vtable
// dispatches to ~Smiley(), which destroys eyes_ and mouth_ before
// control returns to ~Circle() and finally ~Shape().
// ===============================================================================
class Smiley : public Circle {
public:
    Smiley(Point center, double radius) : Circle{center, radius} {}

    void add_eye(std::unique_ptr<Shape> eye) { eyes_.push_back(std::move(eye)); }
    void set_mouth(std::unique_ptr<Shape> mouth) { mouth_ = std::move(mouth); }

    void draw() const override {
        std::cout << "    Smiley    at (" << center_.GetX() << ", " << center_.GetY()
                  << ")  r=" << radius_ << "  eyes=" << eyes_.size() << "\n";
        for (const auto& e : this->eyes_) {
            e->draw();
        }
        if (mouth_) {
            this->mouth_->draw();
        }
    }

    void rotate(int angle) override {
        std::cout << "    Smiley    rotated " << angle << " degrees\n";
        for (auto& e : this->eyes_) {
            e->rotate(angle);
        }
        if (this->mouth_) {
            mouth_->rotate(angle);
        }
    }

    // Traced destructor: shows that virtual dispatch finds ~Smiley()
    // when deleting through Shape* — and that unique_ptrs clean up automatically.
    ~Smiley() override {
        std::cout << "    [dtor] ~Smiley — releasing " << eyes_.size()
                  << " eye(s) and mouth via unique_ptr\n";
    }

private:
    std::vector<std::unique_ptr<Shape>> eyes_;
    std::unique_ptr<Shape> mouth_{nullptr};
};

// ===============================================================================
// Kind + read_shape(): the Stroustrup factory pattern.
//
// Kind encodes the type of shape to construct. read_shape() reads
// a Kind and the required parameters from the stream, constructs
// the correct derived type, and returns it as unique_ptr<Shape>.
// The caller receives ownership through the base-class handle only —
// the concrete type is never exposed beyond this function.
//
// Stream format:
//   0 cx cy r              — Circle
//   1 x1 y1 x2 y2 x3 y3    — Triangle
//   2 cx cy w h            — Rectangle
//   3 cx cy r              — Smiley (eyes and mouth added automatically)
// ===============================================================================
enum class Kind {
    kCircle = 0,
    kTriangle = 1,
    kRectangle = 2,
    kSmiley = 3
};

std::unique_ptr<Shape> read_shape(std::istream& is) {
    int k;
    if (!(is >> k))
        return nullptr;

    switch (static_cast<Kind>(k)) {
        case Kind::kCircle: {
            double cx, cy, r;
            is >> cx >> cy >> r;
            return std::make_unique<Circle>(Point{cx, cy}, r);
        }
        case Kind::kTriangle: {
            double x1, y1, x2, y2, x3, y3;
            is >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
            return std::make_unique<Triangle>(Point{x1, y1}, Point{x2, y2}, Point{x3, y3});
        }
        case Kind::kRectangle: {
            double cx, cy, w, h;
            is >> cx >> cy >> w >> h;
            return std::make_unique<Rectangle>(w, h, cx, cy);
        }
        case Kind::kSmiley: {
            double cx, cy, r;
            is >> cx >> cy >> r;
            auto face = std::make_unique<Smiley>(Point{cx, cy}, r);
            face->add_eye(std::make_unique<Circle>(Point{cx - r / 3, cy + r / 4}, r / 6));
            face->add_eye(std::make_unique<Circle>(Point{cx + r / 3, cy + r / 4}, r / 6));
            face->set_mouth(std::make_unique<Circle>(Point{cx, cy - r / 4}, r / 4));
            return face;
        }
        default:
            throw std::invalid_argument{"read_shape: unknown Kind " + std::to_string(k)};
    }
}

// ===============================================================================
// Free functions operating on the Shape interface
// ===============================================================================
void draw_all(const std::vector<std::unique_ptr<Shape>>& shapes) {
    for (const auto& s : shapes) {
        s->draw();
    }
}

void rotate_all(std::vector<std::unique_ptr<Shape>>& shapes, int angle) {
    for (auto& s : shapes) {
        s->rotate(angle);
    }
}

// ===============================================================================
// user(): the Stroustrup pattern.
//
// Builds a vector<unique_ptr<Shape>> from a stream and applies
// operations. The key improvement over raw pointers:
//
//   Raw pointers (BS original):
//     vector<Shape*> v;
//     ...
//     for (auto p : v) delete p;    // leaks if draw_all() throws
//
//   Smart pointers (improved):
//     vector<unique_ptr<Shape>> v;  // no manual delete needed
//     ...                           // exception-safe by construction
//
// The virtual destructor ensures that when v is destroyed, each
// unique_ptr calls delete on the correct derived type via vtable.
// ===============================================================================
void user(std::istream& is) {
    std::vector<std::unique_ptr<Shape>> shapes{};

    while (is) {
        auto shape = read_shape(is);
        if (!shape) {
            break;
        }
        shapes.push_back(std::move(shape));
    }

    std::cout << "\n  draw_all():\n";
    draw_all(shapes);

    std::cout << "\n  rotate_all(45):\n";
    rotate_all(shapes, 45);

    std::cout << "\n  user() returning — v destroyed, all Shapes freed:\n";
    // unique_ptrs destroyed here in reverse order — virtual dtor called on each
}

// ===============================================================================
// Hierarchy navigation with dynamic_cast — use with restraint.
//
// dynamic_cast<T*>(&s): returns T* if s is a T (or derived from T),
//                       returns nullptr otherwise. Check before use.
//
// dynamic_cast<T&>(s):  succeeds if s is a T, throws std::bad_cast
//                       otherwise. Use when failure is unexpected.
//
// When it IS appropriate:
//   Accessing a feature that belongs only to a specific derived type
//   and genuinely does not make sense in the base interface.
//   radius() on Circle is a canonical example — Triangle has no radius.
//
// When it is a design smell:
//   1. If used frequently, a virtual function is missing from Shape.
//      The right fix is to add it, not to cast everywhere.
//   2. Adding a new derived class requires finding every cast chain —
//      fragile, does not scale with a growing hierarchy.
//   3. It couples the caller to concrete types, defeating the interface.
//   4. It carries a runtime cost (RTTI lookup) vs. a direct virtual call.
//
// Rule of thumb: if you are casting to call something that ALL shapes
// could reasonably do, that thing should be a virtual function instead.
// ===============================================================================
void inspect_shape(const Shape& s) {
    // Check Smiley before Circle — Smiley IS a Circle,
    // so dynamic_cast<Circle*> would also succeed for a Smiley.
    if (const auto* sm = dynamic_cast<const Smiley*>(&s)) {
        std::cout << "  Smiley   — face radius=" << sm->radius() << "\n";
    } else if (const auto* c = dynamic_cast<const Circle*>(&s)) {
        std::cout << "  Circle   — r=" << c->radius()
                  << "  (radius not in Shape — cast justified)\n";
    } else if (dynamic_cast<const Triangle*>(&s)) {
        std::cout << "  Triangle — no radius concept\n";
    } else {
        std::cout << "  other Shape — area=" << s.area() << "\n";
    }

    // Reference form: throws std::bad_cast on failure
    try {
        [[maybe_unused]] const auto& c = dynamic_cast<const Circle&>(s);
        std::cout << "           reference cast succeeded — it IS a Circle\n";
    } catch (const std::bad_cast&) {
        std::cout << "           reference cast failed   — not a Circle\n";
    }
}

int main() {
    // ----------------------------------------------------------
    // Part 1: user() — factory pattern + smart pointers
    // ----------------------------------------------------------
    std::cout << "\n=========== USER() — FACTORY PATTERN ===========\n";
    std::istringstream input{
        "0 3. 4. 5.\n"           // Circle     at (3,4) r=5
        "1 0. 0. 4. 0. 2. 3.\n"  // Triangle   vertices
        "2 1. 1. 6. 3.\n"        // Rectangle  at (1,1) 6x3
        "3 5. 5. 8.\n"           // Smiley     at (5,5) r=8
    };
    user(input);

    // ----------------------------------------------------------
    // Part 2: virtual destructor — delete through Shape*
    // ----------------------------------------------------------
    std::cout << "\n=========== VIRTUAL DESTRUCTOR ===========\n";
    std::cout << "  creating Smiley, assigning to Shape*:\n";
    {
        Shape* ps = new Smiley{Point{0., 0.}, 10.};
        static_cast<Smiley*>(ps)->add_eye(std::make_unique<Circle>(Point{-3., 3.}, 2.));
        static_cast<Smiley*>(ps)->add_eye(std::make_unique<Circle>(Point{3., 3.}, 2.));
        std::cout << "  delete ps (through Shape*):\n";
        delete ps;  // vtable -> ~Smiley()-> eyes_ freed -> ~Circle() -> ~Shape()
    }
    std::cout << "  (without virtual dtor, ~Smiley()would have been skipped)\n";

    // ----------------------------------------------------------
    // Part 3: dynamic_cast — both pointer and reference forms
    // ----------------------------------------------------------
    std::cout << "\n=========== DYNAMIC_CAST: HIERARCHY NAVIGATION ===========\n";
    {
        std::vector<std::unique_ptr<Shape>> shapes;
        shapes.push_back(std::make_unique<Circle>(Point{0., 0.}, 5.));
        shapes.push_back(std::make_unique<Triangle>(Point{0., 0.}, Point{3., 0.}, Point{0., 4.}));
        shapes.push_back(std::make_unique<Rectangle>(6., 3.));
        shapes.push_back(std::make_unique<Smiley>(Point{1., 1.}, 8.));

        for (const auto& s : shapes) {
            inspect_shape(*s);
        }
    }

    return 0;
}
