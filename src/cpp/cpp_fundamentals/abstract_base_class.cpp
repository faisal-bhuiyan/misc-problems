// abstract_base_class.cpp: Faisal Bhuiyan
// Description: Runtime polymorphism through abstract base classes, pure virtual
//              methods, interface inheritance, virtual destructors, and vtables.

#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/*
 * An abstract type completely insulates the user from implementation details.
 * The interface is decoupled from the representation — we do not know (and
 * should not care) how the data is stored. Because of this, abstract types
 * cannot have genuine local variables: we must allocate objects on the heap
 * and access them through references or pointers.
 *
 * A pure virtual function (= 0) declares that a derived class MUST provide
 * an implementation. A class containing any pure virtual method cannot be
 * instantiated — it exists only to define an interface.
 *
 * Interface inheritance: derive from an abstract base class (ABC) containing
 * only pure virtual methods. Callers program to the interface (Container&),
 * not to an implementation (Vector_container). The concrete type is chosen
 * at runtime — this is the runtime nature of this kind of polymorphism.
 *
 * VIRTUAL DESTRUCTOR — best practices:
 *   Any class used as a base through which objects may be deleted MUST have
 *   a virtual destructor. Without it, deleting through a base pointer calls
 *   only the base destructor — derived resources leak. Rules of thumb:
 *     - If a class has any virtual method, give it a virtual destructor
 *     - Prefer = default when the base has no resources of its own
 *     - Use override on derived destructors — catches signature mistakes
 *     - If base-pointer deletion is never intended, use protected ~Base()
 *
 * VTABLE AND OVERHEAD:
 *
 * Every class with virtual functions gets one vtable — a static array of
 * function pointers, one per virtual function, shared by all objects of
 * that class. Every object gets a hidden vptr (8 bytes on 64-bit) pointing
 * to its class's vtable. This is what enables runtime dispatch.
 *
 *   Vector_container object        Vector_container vtable
 *   ┌────────────────────┐        ┌─────────────────────────────────────┐
 *   │ vptr ──────────────┼───────►│ [0]  &Vector_container::operator[]  │
 *   │ v (std::vector)    │        │ [1]  &Vector_container::size()      │
 *   └────────────────────┘        │ [2]  &Vector_container::~Vector_container │
 *                                 └─────────────────────────────────────┘
 *
 *   List_container object          List_container vtable
 *   ┌────────────────────┐        ┌─────────────────────────────────────┐
 *   │ vptr ──────────────┼───────►│ [0]  &List_container::operator[]    │
 *   │ ld (std::list)     │        │ [1]  &List_container::size()        │
 *   └────────────────────┘        │ [2]  &List_container::~List_container │
 *                                 └─────────────────────────────────────┘
 *
 * A virtual call  c->size()  at runtime:
 *   1. Load vptr  from *c           (one memory read)
 *   2. Load fn ptr from vtable[1]   (one memory read)
 *   3. Call through the pointer     (indirect jump — cannot be inlined)
 *
 * A direct call concrete.size() at compile time:
 *   1. Jump directly to known address (zero indirection, can be inlined)
 *
 * Cost: typically 1–2 extra cache misses per virtual call, plus loss of
 * inlining. In tight inner loops this matters. In typical designs (I/O,
 * event dispatch, factory patterns) it is negligible.
 */

// ===============================================================================
// Container: abstract base class (ABC)
//
// Defines the interface only — no data members, no constructor.
// Pure virtual methods force derived classes to implement them.
// Virtual destructor ensures derived destructors are called
// correctly when deleting through a Container pointer.
// ===============================================================================
class Container {
public:
    virtual double& operator[](int i) = 0;  // pure virtual — must override
    virtual int size() const = 0;           // pure virtual — must override

    // = default: base has no resources — compiler generates the body.
    // virtual is essential: without it, delete c where c is Container*
    // would call only ~Container(), leaking derived resources.
    virtual ~Container() = default;
};

// ===============================================================================
// Vector_container: Container backed by a contiguous array.
// operator[] is O(1). Suitable for random access workloads.
// ===============================================================================
class Vector_container : public Container {
public:
    Vector_container(std::initializer_list<double> il) : v{il} {
        std::cout << "    [ctor] Vector_container  sz=" << v.size() << "\n";
    }

    // override: compiler verifies this matches a virtual in the base.
    // Catches typos (e.g. operator[](long) would silently hide, not override).
    ~Vector_container() override { std::cout << "    [dtor] Vector_container\n"; }

    double& operator[](int i) override { return v[i]; }

    int size() const override { return static_cast<int>(v.size()); }

private:
    std::vector<double> v;
};

// ===============================================================================
// List_container: Container backed by a linked list.
// operator[] is O(n) — the interface hides this cost from callers.
// This is the key design tradeoff of abstract interfaces: the
// caller gains flexibility, but loses visibility into complexity.
// ===============================================================================
class List_container : public Container {
public:
    List_container(std::initializer_list<double> il) : ld{il} {
        std::cout << "    [ctor] List_container    sz=" << ld.size() << "\n";
    }

    ~List_container() override { std::cout << "    [dtor] List_container\n"; }

    // O(n): must walk the list to reach index i
    double& operator[](int i) override {
        if (i < 0 || i >= static_cast<int>(ld.size())) {
            throw std::out_of_range{"List_container: index out of range"};
        }
        auto it = ld.begin();
        std::advance(it, i);
        return *it;
    }

    int size() const override { return static_cast<int>(ld.size()); }

private:
    std::list<double> ld;
};

// ===============================================================================
// use(): works with ANY Container — does not know or care
//        whether the backing store is a vector, list, or anything
//        else. This is the payoff of the abstract interface.
//        The correct operator[] is dispatched at runtime via vtable.
// ===============================================================================
void use(Container& c) {
    std::cout << "  use() printing " << c.size() << " elements: ";
    for (int i = 0; i != c.size(); ++i) {
        std::cout << c[i] << " ";
    }
    std::cout << "\n";
}

// g() and h() from the BS example:
// Each creates a concrete container on the stack and passes it
// to use() as a Container reference. The concrete type is visible
// to g() and h() but invisible to use().
void g() {
    Vector_container vc{10., 9., 8., 7., 6.};
    use(vc);
}

void h() {
    List_container lc{1., 2., 3., 4., 5.};
    use(lc);
}

// ===============================================================================
// Factory function: the concrete type is chosen at runtime —
// the caller receives only a Container* and cannot see the type.
// This is where runtime polymorphism is most powerful.
// ===============================================================================
std::unique_ptr<Container> make_container(const std::string& kind) {
    if (kind == "vector") {
        return std::make_unique<Vector_container>(std::initializer_list<double>{1., 2., 3., 4., 5.});
    }
    if (kind == "list") {
        return std::make_unique<List_container>(std::initializer_list<double>{5., 4., 3., 2., 1.});
    }
    throw std::invalid_argument{"unknown container kind: " + kind};
}

// ===============================================================================
// Virtual destructor demonstration.
//
// WITHOUT virtual destructor: deleting through a base pointer
// calls only the base destructor. Derived resources are leaked.
//
// WITH virtual destructor: vtable dispatch calls the derived
// destructor first, then the base destructor — correct cleanup.
// ===============================================================================

class LeakyBase {
public:
    ~LeakyBase() {  // NOT virtual — wrong for a base class
        std::cout << "    [dtor] LeakyBase (derived dtor was SKIPPED)\n";
    }
};

class LeakyDerived : public LeakyBase {
public:
    LeakyDerived() { heap_data = new int[64]; }
    ~LeakyDerived() {  // NEVER CALLED when deleted via base*
        std::cout << "    [dtor] LeakyDerived  (heap_data freed)\n";
        delete[] heap_data;  // this line never runs — memory leaked
    }

private:
    int* heap_data;
};

class SafeBase {
public:
    virtual ~SafeBase() {  // virtual — correct
        std::cout << "    [dtor] SafeBase\n";
    }
};

class SafeDerived : public SafeBase {
public:
    SafeDerived() { heap_data = new int[64]; }
    ~SafeDerived() override {  // called correctly via vtable
        std::cout << "    [dtor] SafeDerived   (heap_data freed)\n";
        delete[] heap_data;
    }

private:
    int* heap_data;
};

// ===============================================================================
// Vtable size overhead: sizeof comparison.
//
// A class with no virtual functions contains only its data.
// A class with virtual functions contains a hidden vptr (8 bytes
// on 64-bit) in addition to its data.
// ===============================================================================
struct Concrete {
    double value;
};  // no vtable

struct Abstract {
    virtual void f() = 0;  // has vtable
    double value;
};

int main() {
    // ----------------------------------------------------------
    // Part 1: g() and h() — BS example
    // ----------------------------------------------------------
    std::cout << "\n=========== g() AND h(): INTERFACE IN ACTION ===========\n";
    std::cout << "\n  g() — Vector_container:\n";
    g();
    std::cout << "\n  h() — List_container:\n";
    h();

    // ----------------------------------------------------------
    // Part 2: runtime dispatch via factory
    // The concrete type is unknown to the caller at compile time.
    // ----------------------------------------------------------
    std::cout << "\n=========== RUNTIME DISPATCH VIA FACTORY ===========\n";
    for (const auto& kind : {"vector", "list"}) {
        std::cout << "\n  make_container(\"" << kind << "\"):\n";
        auto c = make_container(kind);  // Container* — concrete type hidden
        std::cout << "  dispatch to: ";
        use(*c);
        // unique_ptr destructor fires here — virtual dtor ensures
        // the correct derived destructor is called
    }

    // ----------------------------------------------------------
    // Part 3: virtual destructor — right and wrong
    // ----------------------------------------------------------
    std::cout << "\n=========== VIRTUAL DESTRUCTOR ===========\n";

    std::cout << "\n  WITHOUT virtual dtor (LeakyBase):\n";
    {
        LeakyBase* p = new LeakyDerived{};
        delete p;  // only ~LeakyBase() runs — LeakyDerived heap_data leaked
    }

    std::cout << "\n  WITH virtual dtor (SafeBase):\n";
    {
        SafeBase* p = new SafeDerived{};
        delete p;  // ~SafeDerived() runs first, then ~SafeBase() — correct
    }

    // ----------------------------------------------------------
    // Part 4: vtable overhead — sizeof shows the vptr cost
    // ----------------------------------------------------------
    std::cout << "\n=========== VTABLE OVERHEAD ===========\n\n";

    std::cout << "  sizeof(Concrete)         = " << sizeof(Concrete)
              << " bytes  (double only, no vptr)\n";
    std::cout << "  sizeof(Abstract)         = " << sizeof(Abstract)
              << " bytes  (double + 8-byte vptr)\n";
    std::cout << "  sizeof(Container)        = " << sizeof(Container)
              << " bytes  (vptr only — no data members)\n";
    std::cout << "  sizeof(Vector_container) = " << sizeof(Vector_container)
              << " bytes  (vptr + std::vector)\n";
    std::cout << "  sizeof(List_container)   = " << sizeof(List_container)
              << " bytes  (vptr + std::list)\n";

    std::cout << "\n  vptr cost per object: "
              << sizeof(Vector_container) - sizeof(std::vector<double>)
              << " bytes (one pointer to the class vtable)\n";

    // ----------------------------------------------------------
    // Part 5: cannot instantiate an ABC
    // ----------------------------------------------------------
    std::cout << "\n=========== ABC CANNOT BE INSTANTIATED ===========\n";
    std::cout << "  Container c;  would not compile:\n";
    std::cout << "  error: cannot declare variable 'c' to be of abstract type 'Container'\n";
    std::cout << "  note:  because 'Container' has pure virtual methods\n";

    return 0;
}
