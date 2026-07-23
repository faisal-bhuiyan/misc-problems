#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// ------------------------------------------------------------------------------------------
// APPLIED VERSION — construction is centralized in a simple factory
// ------------------------------------------------------------------------------------------
//
// Compare with factor_v1.cpp: clients no longer repeat the mapping from a runtime
// key to a concrete Shape. ShapeFactory owns that decision and returns the common
// Shape interface.
//
// Terminology: this example is a "simple factory." It is not the GoF Factory Method
// pattern, which delegates a virtual creation method to subclasses.
// ------------------------------------------------------------------------------------------

// Product interface: callers use this API regardless of which product was created
class Shape {
public:
    // Required for polymorphic destruction through std::unique_ptr<Shape>
    virtual ~Shape() = default;
    virtual void Draw() const = 0;
};

// Concrete products: their construction is known to ShapeFactory but not to its
// callers
class Circle : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a circle\n"; }
};

class Square : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a square\n"; }
};

class Triangle : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a triangle\n"; }
};

/*
 * The simple factory is the program's single mapping from an external key to a
 * concrete Shape.
 *
 * Benefits:
 *   - Selection logic is not duplicated at every call site.
 *   - Callers depend only on Shape and receive ownership explicitly through
 *     std::unique_ptr.
 *   - Unsupported keys have one consistent failure policy.
 *
 * Limitations:
 *   - The set of products remains closed in this function. Adding a new product
 *     requires modifying Create(), so this is centralization rather than complete
 *     Open/Closed extensibility.
 *   - String keys are convenient at input boundaries but permit invalid values at
 *     runtime. An enum may be preferable for keys produced entirely within the
 *     program.
 *
 * ShapeFactory has no state, so Create() could also be a free function. The class
 * serves only as a namespace for the creation operation in this example.
 */
class ShapeFactory {
public:
    static std::unique_ptr<Shape> Create(const std::string& type) {
        if (type == "circle") {
            return std::make_unique<Circle>();
        } else if (type == "square") {
            return std::make_unique<Square>();
        } else if (type == "triangle") {
            return std::make_unique<Triangle>();
        } else {
            throw std::invalid_argument("Unknown shape type: " + type);
        }
    }
};

int main() {
    // Client code supplies the selection key but does not name or instantiate any
    // concrete product.
    std::unique_ptr<Shape> shape1 = ShapeFactory::Create("circle");
    std::unique_ptr<Shape> shape2 = ShapeFactory::Create("triangle");

    shape1->Draw();
    shape2->Draw();
}

/*
 * For a small, stable product set, this explicit factory is usually the clearest
 * design. See factory-pattern-fundamentals.md for registry-based factories, their
 * initialization and linking costs, and Abstract Factory for related product
 * families.
 */
