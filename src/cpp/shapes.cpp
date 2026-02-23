// shapes.cpp: Faisal Bhuiyan
// Description: A classic example for hierarchy of classes to manage different shapes.

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

/// A simple class to hold 2D points in cartesian space
class Point {
public:
    Point(double x = 0., double y = 0.) {
        coords_.push_back(x);
        coords_.push_back(y);
    }

    double GetX() const { return coords_[0]; }
    double GetY() const { return coords_[1]; }

    void SetX(double x) { coords_[0] = x; }
    void SetY(double y) { coords_[1] = y; }

    double Distance(const Point& pt) const {
        return std::sqrt(std::pow(coords_[0] - pt.GetX(), 2) + std::pow(coords_[1] - pt.GetY(), 2));
    }

    Point& operator=(const Point& pt) {
        coords_[0] = pt.GetX();
        coords_[1] = pt.GetY();
        return *this;
    }

private:
    std::vector<double> coords_;
};

// Let's overload a bunch of operators for objects of the Point class - as is typical,
// such functions are defined outside of the class defintion (since they do not need
// direct access to the representation of Point)
Point operator+(const Point& p1, const Point& p2) {
    return Point{p1.GetX() + p2.GetX(), p1.GetY() + p2.GetY()};
}

Point operator-(const Point& p1, const Point& p2) {
    return Point{p1.GetX() - p2.GetX(), p1.GetY() - p2.GetY()};
}

bool operator==(const Point& p1, const Point& p2) {
    auto close_to = [](double a, double b) -> bool {
        if (a < 1e-6) {
            if (b < 1e-6) {
                return true;
            }
            return false;
        }

        if (((a - b) / a) < 1e-6) {
            return true;
        }
        return false;
    };

    return close_to(p1.GetX(), p2.GetX()) && close_to(p1.GetY(), p2.GetY());
}

bool operator!=(const Point& p1, const Point& p2) {
    return !(p1 == p2);
}

/// An abstract class to define the interface of any arbitrary shape
class Shape {
public:
    // We don't need a ctor for this abstract class, but a dtor is necessary
    virtual ~Shape() = default;

    // Let's add some pure virtual functions to facilitate interface inheritance
    virtual Point Center() const = 0;
    virtual void Move(const Point&) = 0;
    virtual void Rotate(int angle) = 0;
    virtual void Draw() const = 0;
    virtual double Area() const = 0;
};

/// A simple class to manage rectangular shaped objects
class Rectangle : public Shape {
public:
    Rectangle(int width, int height, double x = 0, double y = 0)
        : center_(x, y), width_(width), height_(height) {}

    // Let's assume the points are arranged in a ccw manner
    Rectangle(Point p1, Point p2, Point p3, Point p4) {
        center_ = Point{(p1.GetX() + p2.GetX()) / 2., (p1.GetY() + p2.GetY()) / 2.};
        width_ = p1.Distance(p2);
        height_ = p2.Distance(p3);
    }

    virtual ~Rectangle() = default;

    virtual Point Center() const override { return center_; }

    virtual void Move(const Point& to) override { center_ = to; }

    virtual void Rotate(int angle) override {}  // TBD

    virtual void Draw() const override {}  // TBD

    virtual double Area() const override { return width_ * height_; }

protected:
    Point center_;
    int width_;
    int height_;
};

/// A simple class to manage square shaped objects
class Square : public Rectangle {
public:
    Square(int side) : Rectangle(side, side) {}

    Square(int side, double x, double y) : Rectangle(side, side, x, y) {}
};

/// A simple class to manage circular shaped objects
class Circle : public Shape {
public:
    const double kPI{3.1415'9265'359};

    Circle(double x, double y, int radius) : center_{x, y}, radius_(radius) {}

    Circle(Point pt, int radius) : center_{pt}, radius_(radius) {}

    virtual ~Circle() = default;

    virtual Point Center() const override { return center_; }

    virtual void Move(const Point& to) override { center_ = to; }

    virtual void Rotate(int angle) override {}  // TBD

    virtual void Draw() const override {}  // TBD

    virtual double Area() const override { return kPI * radius_ * radius_; }

protected:
    Point center_;
    int radius_;
};

/// A simple class to manage smileys/emojis that have circular face
class Smiley : public Circle {
public:
    Smiley(Point p, int radius) : Circle{p, radius}, mouth_{nullptr} {}

    virtual void Draw() const override {
        for (auto&& p : eyes_) {
            p->Draw();
        }

        if (mouth_) {
            mouth_->Draw();
        }
    }

    void AddEye(std::unique_ptr<Shape> s) { eyes_.push_back(std::move(s)); }

    void SetMouth(std::unique_ptr<Shape> s) { mouth_ = std::move(s); }

private:
    // Let's add handles to Shape data type here - this is necessary since we should not directly use
    // objects of the base class, which might lead to object slicing
    std::vector<std::unique_ptr<Shape>> eyes_;
    std::unique_ptr<Shape> mouth_;
};

void PrintInformation(const Shape& shape) {
    std::cout << "center: " << shape.Center().GetX() << ", " << shape.Center().GetY() << '\t';
    std::cout << "area: " << shape.Area() << '\n';
}

void DrawAll(std::vector<std::unique_ptr<Shape>>& v) {
    for (auto&& p : v) {
        p->Draw();
    }
}

void RotateAll(std::vector<std::unique_ptr<Shape>>& v, int angle = 0) {
    for (auto&& p : v) {
        p->Rotate(angle);
    }
}

// This is an example user of the Shapes class
void User() {
    std::vector<std::unique_ptr<Shape>> v;
    v.push_back(std::make_unique<Rectangle>(3, 4));
    v.push_back(std::make_unique<Square>(5));
    v.push_back(std::make_unique<Circle>(3, 10, 10));
    v.push_back(std::make_unique<Smiley>(Point{1, 1}, 7));

    DrawAll(v);
    RotateAll(v, 45);
}

int main() {
    Rectangle r{3, 4};
    Circle c{3, 10, 10};
    Square s{5};

    PrintInformation(r);
    PrintInformation(c);
    PrintInformation(s);

    User();
}
