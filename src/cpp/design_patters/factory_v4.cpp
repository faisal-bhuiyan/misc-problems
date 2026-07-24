#include <iostream>
#include <memory>

// ------------------------------------------------------------------------------------------
// APPLIED VERSION — Abstract Factory creates related product families
// ------------------------------------------------------------------------------------------
//
// The earlier examples create one product category: Shape. This example has two
// product categories, Circle and Square, and two coherent families, Wireframe and
// Solid. Selecting one ShapeThemeFactory selects the implementations for both
// product categories.
//
// Abstract Factory is useful when clients must construct several related objects
// without depending on their concrete classes.
// ------------------------------------------------------------------------------------------

// Abstract product interfaces. A family must provide one implementation of each.
class Circle {
public:
    // Required for polymorphic destruction through std::unique_ptr<Circle>.
    virtual ~Circle() = default;
    virtual void Draw() const = 0;
};

class Square {
public:
    // Required for polymorphic destruction through std::unique_ptr<Square>.
    virtual ~Square() = default;
    virtual void Draw() const = 0;
};

// Concrete products in the Wireframe family.
class WireframeCircle : public Circle {
public:
    void Draw() const override { std::cout << "Drawing a wireframe circle\n"; }
};
class WireframeSquare : public Square {
public:
    void Draw() const override { std::cout << "Drawing a wireframe square\n"; }
};

// Concrete products in the Solid family.
class SolidCircle : public Circle {
public:
    void Draw() const override { std::cout << "Drawing a solid-filled circle\n"; }
};
class SolidSquare : public Square {
public:
    void Draw() const override { std::cout << "Drawing a solid-filled square\n"; }
};

/*
 * Abstract Factory interface.
 *
 * Each creation method represents one product category. A concrete factory
 * implements every method with products from the same family. Client code depends
 * on this interface rather than on WireframeCircle, SolidSquare, or any other
 * concrete product.
 */
class ShapeThemeFactory {
public:
    // Required when concrete factories are destroyed through this interface.
    virtual ~ShapeThemeFactory() = default;
    virtual std::unique_ptr<Circle> CreateCircle() const = 0;
    virtual std::unique_ptr<Square> CreateSquare() const = 0;
};

// Produces the complete Wireframe family.
class WireframeThemeFactory : public ShapeThemeFactory {
public:
    std::unique_ptr<Circle> CreateCircle() const override {
        return std::make_unique<WireframeCircle>();
    }
    std::unique_ptr<Square> CreateSquare() const override {
        return std::make_unique<WireframeSquare>();
    }
};

// Produces the complete Solid family.
class SolidThemeFactory : public ShapeThemeFactory {
public:
    std::unique_ptr<Circle> CreateCircle() const override { return std::make_unique<SolidCircle>(); }
    std::unique_ptr<Square> CreateSquare() const override { return std::make_unique<SolidSquare>(); }
};

/*
 * DrawScene obtains both products from one factory. It therefore receives a
 * consistent family as long as each concrete factory satisfies its contract.
 *
 * This reduces accidental mixing, but it does not make mixed families globally
 * impossible: the methods return base-class pointers, and other code could combine
 * products from different factories. Strong type-level enforcement would require
 * family-specific types or an encapsulated aggregate whose members cannot be
 * replaced independently.
 */
void DrawScene(const ShapeThemeFactory& factory) {
    auto circle = factory.CreateCircle();
    auto square = factory.CreateSquare();
    circle->Draw();
    square->Draw();
}

int main() {
    // The client selects a family once and passes its factory to the common
    // workflow. DrawScene is unchanged for either family.
    std::cout << "-- Wireframe theme --\n";
    WireframeThemeFactory wireframe_factory;
    DrawScene(wireframe_factory);

    std::cout << "-- Solid theme --\n";
    SolidThemeFactory solid_factory;
    DrawScene(solid_factory);
}

/*
 * Extension characteristics:
 *   - Adding a new family, such as Textured, requires new concrete products and one
 *     new concrete factory; ShapeThemeFactory and DrawScene can remain unchanged.
 *   - Adding a new product category, such as Triangle, requires a new creation
 *     method on ShapeThemeFactory and corresponding implementations in every
 *     concrete factory. Abstract Factory favors adding families over adding product
 *     categories.
 *
 * The pattern introduces virtual dispatch, dynamic allocation in this example, and
 * several related classes. It is most useful when family consistency and runtime
 * family selection justify that structure.
 */
