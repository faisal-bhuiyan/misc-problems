// scope_lifetime.cpp: Faisal Bhuiyan
// Description: Tracing the scope and lifetime of objects at local, class, namespace, and dynamic
//              (heap) scope levels.

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
 * A destructor marks its end. By tracing these calls, we can
 * observe exactly when objects are born and when they die.
 *
 * C++ defines four main scope levels:
 *   - Local scope:     inside a function or block { }
 *   - Class scope:     inside a class definition — tied to the containing object
 *   - Namespace scope: outside all functions and classes — lives for the whole program
 *   - Dynamic scope:   heap-allocated — lifetime controlled explicitly by the programmer
 */

// ====================================================================
// Tracer: a minimal class whose only purpose is to announce
// its own construction and destruction. Used throughout this
// example to make object lifetimes directly observable.
// ====================================================================
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

// ====================================================================
// NAMESPACE SCOPE
//
// Objects declared here live for the entire duration of the
// program. They are constructed before main() runs and
// destroyed after main() returns, in reverse order of
// construction. This is why global state is dangerous —
// the programmer has very little control over its lifetime.
// ====================================================================
Tracer ns_tracer_first{"namespace: first  (global)"};
Tracer ns_tracer_second{"namespace: second (global)"};

// ====================================================================
// CLASS SCOPE
//
// A member object's lifetime is tied to the lifetime of its
// containing object. Members are constructed in the order they
// are declared and destroyed in reverse order — always.
// The containing object is not considered fully constructed
// until all its members are constructed.
// ====================================================================
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

// ====================================================================
// LOCAL SCOPE
//
// Objects inside a function or block live only as long as the
// block they are declared in. When execution leaves the block
// — whether by a return, an exception, or simply reaching the
// closing brace — destructors are called immediately and in
// reverse order of construction.
// ====================================================================
void demonstrate_local_scope() {
    std::cout << "------- entering function -------\n";
    Tracer fn_tracer{"local: function-scope object"};

    {
        std::cout << "\n  ------- entering nested block -------\n";
        Tracer block_tracer{"local: nested-block object"};
        std::cout << "\n  both local objects alive here\n";
        std::cout << "\n  ------- leaving nested block -------\n";
    }  // block_tracer destroyed HERE — fn_tracer still alive

    std::cout << "\n  only fn_tracer still alive\n";
    std::cout << "\n  ------- leaving function -------\n";
}  // fn_tracer destroyed HERE

// ====================================================================
// DYNAMIC SCOPE
//
// Heap-allocated objects have their lifetime controlled
// explicitly by the programmer via new and delete.
// The key danger: if delete is never called (e.g. due to an
// early return or exception), the object leaks.
//
// The modern C++ solution is unique_ptr — an RAII wrapper that
// calls delete in its destructor automatically, restoring the
// same safety guarantee as local objects.
// ====================================================================
void demonstrate_dynamic_scope() {
    std::cout << "------- raw new/delete -------\n";
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
    }  // unique_ptr destructor runs HERE — calls delete automatically
    std::cout << "\n  managed object gone — no manual delete needed\n";
}

int main() {
    std::cout << "\n=========== main() entered ===========\n";
    std::cout << "(namespace-scope objects were already alive before this line)\n";

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
    std::cout << "(namespace-scope objects will die after this)\n";

    return 0;
}

// ns_tracer_second destroyed first  -> reverse of construction order
// ns_tracer_first  destroyed second
