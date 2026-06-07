// scope_and_lifetime.cpp: Faisal Bhuiyan
// Description: Tracing the scope and lifetime of objects at local, class, namespace, global, and
//              dynamic (heap) scope levels.

#include <iostream>
#include <memory>
#include <string>

/*
 * Every variable or object in C++ has a scope and a lifetime.
 *
 * Scope:    the region of program text in which the name can be used.
 * Lifetime: the period during which the object exists in memory.
 *
 * A constructor marks the beginning of an object's lifetime.
 * A destructor marks its end. By tracing constructor and destructor
 * calls, we can observe exactly when objects are born and when they die.
 *
 * The scope levels demonstrated here:
 *
 *   Global scope:    a name not declared inside any other construct. It
 *                    belongs to the global namespace and is accessible
 *                    by name from anywhere in the program.
 *
 *   Namespace scope: a name defined in a named namespace, outside any
 *                    function, lambda, class, or enum class. Its scope
 *                    extends from the point of declaration to the end of
 *                    its namespace. Must be accessed via the :: qualifier.
 *
 *   Class scope:     a member name — tied to the lifetime of the object
 *                    that contains it.
 *
 *   Local scope:     a name declared inside a function or block { }.
 *                    Destroyed immediately when its block closes.
 *
 *   Dynamic scope:   heap-allocated — lifetime controlled explicitly
 *                    by the programmer via new/delete or unique_ptr.
 */

// ===============================================================================
// Tracer: a minimal class whose only purpose is to announce
// its own construction and destruction. Used throughout this
// example to make object lifetimes directly observable.
// ===============================================================================
class Tracer {
public:
    explicit Tracer(const std::string& name) : name_{name} {
        std::cout << "    [BORN]  " << name_ << "\n";
    }

    ~Tracer() { std::cout << "    [DIED]  " << name_ << "\n"; }

    inline const std::string& name() const { return name_; }

private:
    std::string name_;
};

// ===============================================================================
// GLOBAL SCOPE
//
// Not declared inside any namespace, function, lambda, class,
// or enum class. global_tracer belongs to the global namespace
// and is accessible by its bare name from anywhere in the program.
//
// Constructed before main() runs. Destroyed after main() returns.
// ===============================================================================
Tracer global_tracer{"global scope: tracer"};

// ===============================================================================
// NAMESPACE SCOPE
//
// A name defined inside a named namespace, outside any function,
// lambda, class, or enum class, is a namespace member name.
// Its scope extends from the point of declaration to the end
// of the namespace.
//
// Unlike global names, namespace member names require the ::
// qualifier to access — Physics::gravity, Physics::ns_tracer —
// unless explicitly brought into scope with a using declaration.
//
// Like global objects, namespace-scope objects are constructed
// before main() and destroyed after it returns, in reverse order
// of construction within their translation unit.
// ===============================================================================
namespace Physics {

Tracer ns_tracer{"Physics namespace scope: tracer"};

// These are all namespace member names — their scope begins
// at the point of declaration and ends at the closing brace.
constexpr double gravity{9.81};        // Physics::gravity
constexpr double speed_of_light{3e8};  // Physics::speed_of_light

double kinetic_energy(double mass, double v) {
    return 0.5 * mass * v * v;
}

}  // end of namespace Physics
   // Physics::ns_tracer is not accessible as bare 'ns_tracer' beyond this point
   // without a using declaration.

// ===============================================================================
// CLASS SCOPE
//
// A member object's lifetime is tied to its containing object.
// Members are constructed in declaration order and destroyed in
// reverse order. The object is not fully constructed until all
// its members are constructed.
// ===============================================================================
class Engine {
public:
    Engine(const std::string& id)
        : id_{id},
          primary_{"class: Engine(" + id + ")::primary_"},
          secondary_{"class: Engine(" + id + ")::secondary_"} {
        std::cout << "    Engine(" << id_ << ") fully constructed\n";
    }

    ~Engine() {
        std::cout << "    Engine(" << id_ << ") body destroyed"
                  << " — members will follow in reverse order\n";
    }

private:
    std::string id_;
    Tracer primary_;    // constructed first  -> destroyed last
    Tracer secondary_;  // constructed second -> destroyed first
};

// ===============================================================================
// LOCAL SCOPE
//
// Objects inside a function or block live only as long as that
// block. When execution leaves — by return, exception, or closing
// brace — destructors fire immediately, in reverse construction order.
// ===============================================================================
void demonstrate_local_scope() {
    std::cout << "  ------- entering function -------\n";
    Tracer fn_tracer{"local: function-scope object"};

    {
        std::cout << "\n  ------- entering nested block -------\n";
        Tracer block_tracer{"local: nested-block object"};
        std::cout << "  both local objects alive here\n";
        std::cout << "\n  ------- leaving nested block -------\n";
    }  // block_tracer destroyed HERE — fn_tracer still alive

    std::cout << "\n  only fn_tracer still alive\n";
    std::cout << "\n  ------- leaving function -------\n";
}  // fn_tracer destroyed HERE

// ===============================================================================
// DYNAMIC SCOPE
//
// Heap-allocated objects have their lifetime controlled explicitly
// via new/delete. If delete is never called — due to an early
// return or thrown exception — the object leaks.
//
// unique_ptr is an RAII wrapper that restores the same safety
// guarantee as a local object: delete is called automatically
// in the destructor, even if an exception is thrown.
// ===============================================================================
void demonstrate_dynamic_scope() {
    std::cout << "  ------- raw new/delete -------\n";
    Tracer* raw = new Tracer{"dynamic: raw pointer object"};
    std::cout << "  alive on heap — programmer must delete manually\n";
    delete raw;  // lifetime ends HERE — explicit and manual
    raw = nullptr;
    std::cout << "  raw object deleted\n";

    std::cout << "\n  ------- unique_ptr (RAII wrapper over heap) -------\n";
    {
        auto managed = std::make_unique<Tracer>("dynamic: unique_ptr object");
        std::cout << "  managed object alive on heap\n";
        std::cout << "\n  ------- leaving unique_ptr block -------\n";
    }  // unique_ptr destructor fires HERE — calls delete automatically
    std::cout << "\n  managed object gone — no manual delete needed\n";
}

int main() {
    std::cout << "\n=========== main() entered ===========\n";
    std::cout << "(global and namespace-scope objects already alive)\n";

    // ----------------------------------------------------------
    // Demonstrate namespace scope access rules
    // ----------------------------------------------------------
    std::cout << "\n=========== NAMESPACE SCOPE ACCESS ===========\n";
    std::cout << "  Physics::gravity        = " << Physics::gravity << "\n";
    std::cout << "  Physics::speed_of_light = " << Physics::speed_of_light << "\n";
    std::cout << "  Physics::kinetic_energy(10.0, 5.0) = " << Physics::kinetic_energy(10., 5.)
              << "\n";

    // A using declaration brings a single name into local scope
    {
        using Physics::kinetic_energy;
        double ke = kinetic_energy(2., 3.);  // no qualifier needed now
        std::cout << "  after using Physics::kinetic_energy: ke = " << ke << "\n";
    }  // 'kinetic_energy' without qualifier no longer in scope here

    // ----------------------------------------------------------
    // Local scope
    // ----------------------------------------------------------
    std::cout << "\n=========== LOCAL SCOPE ===========\n";
    demonstrate_local_scope();

    // ----------------------------------------------------------
    // Class scope
    // ----------------------------------------------------------
    std::cout << "\n=========== CLASS SCOPE ===========\n";
    {
        std::cout << "  ------- constructing Engine -------\n";
        Engine e{"A"};
        std::cout << "\n  Engine alive — all members alive\n";
        std::cout << "\n  ------- leaving Engine block -------\n";
    }  // Engine body destructor runs, then members in reverse order

    // ----------------------------------------------------------
    // Dynamic scope
    // ----------------------------------------------------------
    std::cout << "\n=========== DYNAMIC SCOPE ===========\n";
    demonstrate_dynamic_scope();

    std::cout << "\n=========== main() returning ===========\n";
    std::cout << "(global and namespace-scope objects will die after this)\n";

    return 0;
}

// Destruction after main() — reverse of construction order:
//   Physics::ns_tracer  destroyed first  (declared second)
//   global_tracer       destroyed second (declared first)
