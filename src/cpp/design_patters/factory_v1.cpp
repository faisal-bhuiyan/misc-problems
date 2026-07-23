#include <iostream>
#include <memory>
#include <string>

// ------------------------------------------------------------------------------------------
// NAIVE VERSION — object-selection logic is embedded in the client
// ------------------------------------------------------------------------------------------
//
// This is the starting point for the Factory tutorial. The Shape hierarchy already
// provides runtime polymorphism, so client code can use Shape::Draw() without
// knowing which override executes. Construction remains coupled to the concrete
// classes, however: the client must decide whether to instantiate Circle or Square.
//
// Compare this file with factor_v2.cpp, where that decision is moved into a simple
// factory.
// ------------------------------------------------------------------------------------------

// Product interface: the common API exposed to clients after construction
class Shape {
public:
    // Required because Shape objects are destroyed polymorphically through std::unique_ptr<Shape>
    virtual ~Shape() = default;
    virtual void Draw() const = 0;
};

// Concrete products
class Circle : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a circle\n"; }
};

class Square : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a square\n"; }
};

int main() {
    // In a real program this value might come from user input or configuration.
    std::string type = "circle";

    // The client performs two unrelated tasks:
    //   1. use a Shape through its abstract interface; and
    //   2. map an external key to a concrete Shape implementation.
    //
    // Consequences:
    //   - Every creation site must include and name each concrete Shape class.
    //   - The selection branch is likely to be duplicated across the program.
    //   - Adding another Shape requires every duplicated branch to be updated.
    //   - An unknown key leaves shape null; dereferencing it below would be
    //     undefined behavior.
    std::unique_ptr<Shape> shape;
    if (type == "circle") {
        shape = std::make_unique<Circle>();
    } else if (type == "square") {
        shape = std::make_unique<Square>();
    }

    shape->Draw();
}

/*
 * factor_v2.cpp centralizes this key-to-type mapping in one simple factory.
 * The factory does not remove knowledge of the concrete types from the program;
 * it assigns that knowledge to one component and gives all callers a consistent
 * construction API.
 */
