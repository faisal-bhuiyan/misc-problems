#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

// ------------------------------------------------------------------------------------------
// APPLIED VERSION — a registry-based, self-registering factory
// ------------------------------------------------------------------------------------------
//
// Compare with factory_v2.cpp: the explicit if/else chain has been replaced by a
// registry that maps string keys to creator functions. Concrete products register
// their own creators, so ShapeFactory does not change when another Shape is added.
//
// This design supports decentralized extension, but it also introduces process-wide
// state, dynamic initialization, and less-visible construction rules. It should be
// used when independent registration is a real requirement, not merely to avoid a
// small and stable branch.
// ------------------------------------------------------------------------------------------

// Product interface returned by every registered creator.
class Shape {
public:
    // Required for polymorphic destruction through std::unique_ptr<Shape>.
    virtual ~Shape() = default;
    virtual void Draw() const = 0;
};

/*
 * ShapeFactory stores one creator function for each external key.
 *
 * Creator erases the concrete product type behind a common callable signature:
 * invoking any registered creator returns an owning std::unique_ptr<Shape>.
 *
 * Instance() uses a function-local static so the registry is initialized on first
 * use. Its initialization is thread-safe in C++11 and later. Mutation of creators_
 * is not synchronized, however, so registration must finish before concurrent calls
 * to Create(), unless synchronization is added.
 */
class ShapeFactory {
public:
    using Creator = std::function<std::unique_ptr<Shape>()>;

    // Return the one process-wide registry.
    static ShapeFactory& Instance() {
        static ShapeFactory instance;
        return instance;
    }

    // Reject duplicate keys rather than silently replacing an existing creator.
    void Register(const std::string& key, Creator creator) {
        const bool inserted = creators_.emplace(key, std::move(creator)).second;
        if (!inserted) {
            throw std::logic_error("Duplicate shape type: " + key);
        }
    }

    // Construct the product associated with key and transfer ownership to the caller.
    std::unique_ptr<Shape> Create(const std::string& key) const {
        const auto it = creators_.find(key);
        if (it == creators_.end()) {
            throw std::invalid_argument("Unknown shape type: " + key);
        }
        return it->second();
    }

private:
    ShapeFactory() = default;
    ShapeFactory(const ShapeFactory&) = delete;
    ShapeFactory& operator=(const ShapeFactory&) = delete;

    std::unordered_map<std::string, Creator> creators_;
};

/*
 * AutoRegister connects static initialization to the registry. Each concrete Shape
 * declares an inline static AutoRegister member whose constructor runs before
 * main() and installs that Shape's creator.
 */
struct AutoRegister {
    AutoRegister(const std::string& key, ShapeFactory::Creator creator) {
        ShapeFactory::Instance().Register(key, std::move(creator));
    }
};

// Each concrete product contains its own key-to-creator registration. The lambda
// constructs the concrete type, while Creator converts the result to
// std::unique_ptr<Shape>.
class Circle : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a circle\n"; }

private:
    static inline AutoRegister registered_{
        "circle", [] { return std::make_unique<Circle>(); }};
};

class Square : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a square\n"; }

private:
    static inline AutoRegister registered_{
        "square", [] { return std::make_unique<Square>(); }};
};

class Triangle : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a triangle\n"; }

private:
    static inline AutoRegister registered_{
        "triangle", [] { return std::make_unique<Triangle>(); }};
};

// Pentagon and its registration can be added without modifying ShapeFactory.
// Selection code that explicitly requests "pentagon" may still need to change;
// configuration-driven callers do not require a new branch.
class Pentagon : public Shape {
public:
    void Draw() const override { std::cout << "Drawing a pentagon\n"; }

private:
    static inline AutoRegister registered_{
        "pentagon", [] { return std::make_unique<Pentagon>(); }};
};

int main() {
    auto& factory = ShapeFactory::Instance();

    // The caller knows only the external key and the Shape interface. Selection is
    // performed by a registry lookup rather than by a client-side branch.
    factory.Create("circle")->Draw();
    factory.Create("square")->Draw();
    factory.Create("pentagon")->Draw();
}

/*
 * Important production considerations:
 *   - Registration occurs during dynamic initialization. Registrars should not
 *     depend on initialization order across translation units.
 *   - Object files containing only registration side effects may be discarded when
 *     linked from static libraries unless they are explicitly retained.
 *   - The process-wide registry is hidden global state and can complicate isolated
 *     tests; dependency injection may be preferable when multiple registries are
 *     required.
 *   - For a small, fixed set of Shapes, the explicit simple factory in
 *     factory_v2.cpp remains easier to inspect and debug.
 */
