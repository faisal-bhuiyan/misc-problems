# Factory Patterns in C++: Fundamentals

Strategy and Factory address different concerns. Strategy selects how an operation
is performed; a factory centralizes how an object is selected and constructed.

> When object construction depends on runtime information—such as configuration,
> user input, or a target platform—where should that selection logic reside, and
> how can it be kept consistent across call sites?

This tutorial develops three related techniques:

1. a simple factory function that centralizes construction;
2. a registry-based factory that supports decentralized registration; and
3. Abstract Factory, which creates families of related products.

The term **Factory Method** has a narrower GoF meaning: a virtual creation operation
that subclasses override. A standalone function such as
`CreatePaymentStrategy(...)` is commonly called a **simple factory**, although
“simple factory” is not one of the original GoF patterns.

---

## Part 1 — The problem: duplicated construction logic

Assume that `Order` already uses Strategy: it depends on the
`PaymentStrategy` interface rather than on concrete payment classes. Some component
must still translate an external value such as `"paypal"` into a
`PayPalPayment`. If every call site performs this translation independently, the
construction logic is duplicated:

```cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void Pay(double amount) const = 0;
};

class CreditCardPayment : public PaymentStrategy {
public:
    void Pay(double amount) const override {
        std::cout << "Charging $" << amount << " to credit card...\n";
    }
};

class PayPalPayment : public PaymentStrategy {
public:
    void Pay(double amount) const override {
        std::cout << "Redirecting to PayPal for $" << amount << "...\n";
    }
};

class Order {
public:
    Order(double amount, std::unique_ptr<PaymentStrategy> strategy)
        : amount_{amount}, strategy_{std::move(strategy)} {}
    void Checkout() const { strategy_->Pay(amount_); }
private:
    double amount_;
    std::unique_ptr<PaymentStrategy> strategy_;
};

// Web checkout handler.
std::unique_ptr<PaymentStrategy> HandleWebCheckout(const std::string& choice) {
    if (choice == "credit_card") return std::make_unique<CreditCardPayment>();
    if (choice == "paypal")      return std::make_unique<PayPalPayment>();
    throw std::invalid_argument("Unknown payment type: " + choice);
}

// Mobile checkout handler: the same selection logic is duplicated.
std::unique_ptr<PaymentStrategy> HandleMobileCheckout(const std::string& choice) {
    if (choice == "credit_card") return std::make_unique<CreditCardPayment>();
    if (choice == "paypal")      return std::make_unique<PayPalPayment>();
    throw std::invalid_argument("Unknown payment type: " + choice);
}

int main() {
    Order web_order{49.99, HandleWebCheckout("paypal")};
    web_order.Checkout();

    Order mobile_order{19.99, HandleMobileCheckout("credit_card")};
    mobile_order.Checkout();
}
```

```
Redirecting to PayPal for $49.99...
Charging $19.99 to credit card...
```

Adding another checkout path or payment method requires several conditionals to be
updated consistently. Strategy removed concrete payment types from `Order`, but it
did not determine where concrete strategies should be selected and constructed.

---

## Part 2 — Simple factory: centralizing construction

The direct solution is to move the selection logic into one function used by every
call site.

```cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// ... PaymentStrategy, CreditCardPayment, PayPalPayment, Order unchanged ...

// The only function that maps an external key to a concrete strategy.
std::unique_ptr<PaymentStrategy> CreatePaymentStrategy(const std::string& choice) {
    if (choice == "credit_card") return std::make_unique<CreditCardPayment>();
    if (choice == "paypal")      return std::make_unique<PayPalPayment>();
    throw std::invalid_argument("Unknown payment type: " + choice);
}

int main() {
    Order web_order{49.99, CreatePaymentStrategy("paypal")};
    web_order.Checkout();

    Order mobile_order{19.99, CreatePaymentStrategy("credit_card")};
    mobile_order.Checkout();
}
```

```
Redirecting to PayPal for $49.99...
Charging $19.99 to credit card...
```

This design removes duplication and establishes one authoritative mapping from keys
to concrete types. It also gives the program one place to define error handling for
unsupported keys.

The function still contains a closed list of alternatives. Adding
`BitcoinLightningPayment`, for example, requires modifying
`CreatePaymentStrategy`. This is often appropriate for a small, stable set of types:
the explicit branch is easy to inspect, debug, and test. A registry is useful only
when independent extension is an actual requirement.

---

## Part 3 — Registry-based factory

A registry replaces the explicit branch with a mapping from keys to creator
functions. Concrete types can register their creators independently, allowing the
factory implementation to remain unchanged as new types are added.

```cpp
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void Pay(double amount) const = 0;
};

// Maps an external key to a function that constructs a strategy.
class PaymentStrategyFactory {
public:
    using Creator = std::function<std::unique_ptr<PaymentStrategy>()>;

    static PaymentStrategyFactory& Instance() {
        static PaymentStrategyFactory instance;
        return instance;
    }

    void Register(const std::string& key, Creator creator) {
        const bool inserted = creators_.emplace(key, std::move(creator)).second;
        if (!inserted) {
            throw std::logic_error("Duplicate payment type: " + key);
        }
    }

    std::unique_ptr<PaymentStrategy> Create(const std::string& key) const {
        auto it = creators_.find(key);
        if (it == creators_.end()) {
            throw std::invalid_argument("Unknown payment type: " + key);
        }
        return it->second();
    }

private:
    PaymentStrategyFactory() = default;
    PaymentStrategyFactory(const PaymentStrategyFactory&) = delete;
    PaymentStrategyFactory& operator=(const PaymentStrategyFactory&) = delete;

    std::unordered_map<std::string, Creator> creators_;
};

// A static instance of this helper registers a creator during initialization.
struct AutoRegister {
    AutoRegister(const std::string& key, PaymentStrategyFactory::Creator creator) {
        PaymentStrategyFactory::Instance().Register(key, std::move(creator));
    }
};

// CreditCardPayment registers its creator under "credit_card".
class CreditCardPayment : public PaymentStrategy {
public:
    void Pay(double amount) const override {
        std::cout << "Charging $" << amount << " to credit card...\n";
    }
private:
    static inline AutoRegister registered_{
        "credit_card", [] { return std::make_unique<CreditCardPayment>(); }
    };
};

// PayPalPayment registers its creator under "paypal".
class PayPalPayment : public PaymentStrategy {
public:
    void Pay(double amount) const override {
        std::cout << "Redirecting to PayPal for $" << amount << "...\n";
    }
private:
    static inline AutoRegister registered_{
        "paypal", [] { return std::make_unique<PayPalPayment>(); }
    };
};

// CryptoPayment can be added without modifying PaymentStrategyFactory.
class CryptoPayment : public PaymentStrategy {
public:
    void Pay(double amount) const override {
        std::cout << "Generating wallet address for $" << amount << "...\n";
    }
private:
    static inline AutoRegister registered_{
        "crypto", [] { return std::make_unique<CryptoPayment>(); }
    };
};

class Order {
public:
    Order(double amount, std::unique_ptr<PaymentStrategy> strategy)
        : amount_{amount}, strategy_{std::move(strategy)} {}
    void Checkout() const { strategy_->Pay(amount_); }
private:
    double amount_;
    std::unique_ptr<PaymentStrategy> strategy_;
};

int main() {
    auto& factory = PaymentStrategyFactory::Instance();

    Order web_order{49.99, factory.Create("paypal")};
    web_order.Checkout();

    Order mobile_order{19.99, factory.Create("credit_card")};
    mobile_order.Checkout();

    Order kiosk_order{9.99, factory.Create("crypto")};
    kiosk_order.Checkout();
}
```

```
Redirecting to PayPal for $49.99...
Charging $19.99 to credit card...
Generating wallet address for $9.99...
```

**Mechanism:**
- The selection is represented as registry data and performed with a lookup rather
  than with a branch in the factory.
- Each `static inline AutoRegister` object invokes `Register` during dynamic
  initialization. The function-local static in `Instance()` ensures that the
  factory itself is initialized before the first registration attempt.
- A concrete payment type and its registration can be added without modifying
  `PaymentStrategyFactory`. Code that explicitly requests the new key may still
  require modification; configuration-driven selection does not require changes to
  the caller's selection logic.

**Costs and constraints:**
- **Global state.** `Instance()` makes the registry process-wide. This can be
  acceptable for a process-wide type catalog, but it introduces hidden coupling and
  makes isolated tests more difficult. Passing a registry explicitly is preferable
  when different configurations or test substitutes are required.
- **Initialization and linking.** Dynamic initialization across translation units
  has an unspecified order. This example does not depend on registration order, and
  the function-local factory avoids one common initialization-order problem.
  Nevertheless, duplicate keys, dependencies among registrars, and teardown order
  require deliberate policies. In addition, object files that contain only
  registration side effects may be omitted when linking static libraries unless
  they are otherwise referenced or explicitly retained.
- **Concurrency.** Initialization of the function-local static is thread-safe, but
  mutation of `creators_` is not synchronized. Registration should finish before
  concurrent calls to `Create`, or the registry must provide synchronization.
- **Discoverability.** Construction rules are distributed among registrars rather
  than listed in one function. This improves decentralized extensibility but makes
  the complete set of available keys less obvious.
- **Failure timing.** In this example, a duplicate key throws during static
  initialization and therefore prevents normal program startup. Production code
  should define an explicit duplicate-registration and error-reporting policy.

For a small and stable set of alternatives, the simple factory in Part 2 is usually
clearer.

---

## Part 4 — Abstract Factory: creating related product families

The factories in Parts 2 and 3 create one category of product:
`PaymentStrategy`. Abstract Factory addresses a different requirement: constructing
several related product categories from the same family.

For example, a solver and its preconditioner may need to use the same execution
backend. Independently constructing a `GpuSolver` and a `CpuPreconditioner` could
produce an invalid configuration involving incompatible memory spaces. A backend
factory centralizes the creation of compatible components and allows client code to
obtain both from one selected family.

```cpp
#include <iostream>
#include <memory>

// Two product hierarchies whose implementations must use the same backend.
class Solver {
public:
    virtual ~Solver() = default;
    virtual void Solve() const = 0;
};

class Preconditioner {
public:
    virtual ~Preconditioner() = default;
    virtual void Apply() const = 0;
};

class CpuSolver : public Solver {
public:
    void Solve() const override { std::cout << "Solving on CPU (threaded)...\n"; }
};
class CpuPreconditioner : public Preconditioner {
public:
    void Apply() const override { std::cout << "Applying CPU preconditioner...\n"; }
};

class GpuSolver : public Solver {
public:
    void Solve() const override { std::cout << "Solving on GPU (Kokkos kernel)...\n"; }
};
class GpuPreconditioner : public Preconditioner {
public:
    void Apply() const override { std::cout << "Applying GPU preconditioner...\n"; }
};

// Abstract Factory interface for a family of backend-specific products.
class BackendFactory {
public:
    virtual ~BackendFactory() = default;
    virtual std::unique_ptr<Solver> CreateSolver() const = 0;
    virtual std::unique_ptr<Preconditioner> CreatePreconditioner() const = 0;
};

class CpuBackendFactory : public BackendFactory {
public:
    std::unique_ptr<Solver> CreateSolver() const override {
        return std::make_unique<CpuSolver>();
    }
    std::unique_ptr<Preconditioner> CreatePreconditioner() const override {
        return std::make_unique<CpuPreconditioner>();
    }
};

class GpuBackendFactory : public BackendFactory {
public:
    std::unique_ptr<Solver> CreateSolver() const override {
        return std::make_unique<GpuSolver>();
    }
    std::unique_ptr<Preconditioner> CreatePreconditioner() const override {
        return std::make_unique<GpuPreconditioner>();
    }
};

// This client obtains both products from the same selected factory.
void RunSimulation(const BackendFactory& factory) {
    auto solver = factory.CreateSolver();
    auto preconditioner = factory.CreatePreconditioner();
    preconditioner->Apply();
    solver->Solve();
}

int main() {
    std::cout << "-- CPU backend --\n";
    CpuBackendFactory cpu_factory;
    RunSimulation(cpu_factory);

    std::cout << "-- GPU backend --\n";
    GpuBackendFactory gpu_factory;
    RunSimulation(gpu_factory);
}
```

```
-- CPU backend --
Applying CPU preconditioner...
Solving on CPU (threaded)...
-- GPU backend --
Applying GPU preconditioner...
Solving on GPU (Kokkos kernel)...
```

`RunSimulation` does not construct products directly. It asks one
`BackendFactory` for both, so this function receives a consistent family as long as
each concrete factory satisfies its contract.

This example reduces the opportunity to mix backends, but it does not make mismatch
globally impossible at the type level: both factory methods return base-class
pointers, and other client code could still combine products from different
factories. If compile-time enforcement is required, represent the backend in the
product types or return an encapsulated backend-specific aggregate that does not
allow its components to be replaced independently.

The distinctions are:
- a **simple factory** centralizes selection and creation for one product category;
- a **GoF Factory Method** is a creation operation overridden by subclasses; and
- an **Abstract Factory** exposes several creation operations for a related family
  of product categories.

---

## Part 5 — Decision framework

- Use a **simple factory function** when the set of alternatives is small, stable,
  and controlled by one component.
- Use a **registry-based factory** when implementations must register independently
  and the benefits justify global registration, initialization, and discoverability
  concerns.
- Consider **`std::variant`** when the alternatives form a closed set that should be
  represented directly in the type system. A variant does not itself replace
  construction logic, but it can remove the need for a polymorphic base class.
- Use **Factory Method** when a base workflow delegates creation of a product to
  subclasses.
- Use **Abstract Factory** when client code must create several related product
  categories from one selected family.

In summary, simple and registry-based factories centralize the selection of a
concrete product. Factory Method delegates creation to subclasses. Abstract Factory
coordinates the creation of related product categories.

---

## Exercises

1. **Add a registered product.** In Part 3, add a
   `BitcoinLightningPayment` type. Confirm that
   `PaymentStrategyFactory` and `Order` do not require modification. Exercise the
   new registration through external input or add a caller that requests its key.

2. **Compare key types.** Replace the registry's `std::string` keys with a
   `PaymentType` enumeration. Identify the compile-time guarantees this provides and
   the parsing step required when a key originates outside the program.

3. **Extend an Abstract Factory.** Add a third backend, such as a
   `HybridBackendFactory` that creates a CPU solver and a GPU-accelerated
   preconditioner, provided that combination is valid in the domain. Determine which
   interfaces remain unchanged and which build or selection code must know about the
   new factory.

4. **Apply the registry pattern.** Sketch a registry of file-format importers for a
   geometry library. Define the creator signature, registration lifecycle,
   duplicate-key policy, behavior for unknown formats, and mechanism that ensures
   registrar object files are retained by the linker.
