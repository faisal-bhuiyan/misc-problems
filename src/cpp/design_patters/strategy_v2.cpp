#include <iostream>
#include <memory>

// ------------------------------------------------------------------------------------------
// APPLIED VERSION — the Strategy pattern (GoF, runtime form)
// ------------------------------------------------------------------------------------------
//
// Compare against strategy_naive.cpp / strategy_v1.cpp: the if/else ladder
// that lived inside Order::Checkout has been extracted into its own
// hierarchy, and Order now depends on an abstraction instead of on every
// concrete payment method by name.
// ------------------------------------------------------------------------------------------

/*
 * The interface for "the algorithm that varies" -- how a payment actually happens
 * This single virtual method is the seam the naive version was missing: it's
 * the one thing every payment method has in common, and the only thing Order
 * needs to know about.
 */
class PaymentStrategy {
public:
    // Destructor is virtual to allow for polymorphic destruction
    virtual ~PaymentStrategy() = default;

    // Pure virtual function that must be implemented by concrete strategies
    virtual void Pay(double amount) const = 0;
};

/*
 * Each concrete strategy owns ONLY its own payment logic. Adding "ApplePay"
 * tomorrow means writing one new class here -- Order and every other
 * strategy are untouched (Open/Closed).
 */
class CreditCardPayment : public PaymentStrategy {
public:
    // Implementation of the Pay method for credit card payment
    void Pay(double amount) const override {
        std::cout << "Charging $" << amount << " to credit card...\n";
    }
};

class PayPalPayment : public PaymentStrategy {
public:
    // Implementation of the Pay method for PayPal payment
    void Pay(double amount) const override {
        std::cout << "Redirecting to PayPal for $" << amount << "...\n";
    }
};

class CryptoPayment : public PaymentStrategy {
public:
    // Implementation of the Pay method for crypto payment
    void Pay(double amount) const override {
        std::cout << "Generating wallet address for $" << amount << "...\n";
    }
};

/*
 * The "context" -- holds a strategy, delegates to it, knows NOTHING about
 * how any specific payment method actually works
 *
 * This is the fix for every pain point in the naive version:
 *   - No if/else chain, and no chain to grow when a new method is added.
 *   - No bare string to typo -- the compiler enforces that whatever is
 *     passed in is a real PaymentStrategy.
 *   - Order is trivially testable in isolation: inject a mock/fake
 *     PaymentStrategy and assert Pay() was called, with no real card
 *     validation or OAuth calls involved.
 *   - Order's code and "how PayPal works" can change independently and be
 *     understood independently.
 */
class Order {
public:
    /**
     * @brief Constructor that takes an amount and a strategy
     * @param amount The amount of the order
     * @param strategy The payment strategy
     */
    Order(double amount, std::unique_ptr<PaymentStrategy> strategy)
        : amount_{amount}, strategy_{std::move(strategy)} {}

    /**
     * @brief Set the payment strategy
     *
     * @note Strategy can be swapped on an existing object at runtime -- something
     * the naive version's hardcoded if/else couldn't express at all without
     * re-entering Checkout() with a different string.
     */
    void SetPaymentStrategy(std::unique_ptr<PaymentStrategy> strategy) {
        this->strategy_ = std::move(strategy);
    }

    /**
     * @brief Checkout the order
     */
    void Checkout() const { this->strategy_->Pay(this->amount_); }

private:
    double amount_;                              ///< Amount of the order
    std::unique_ptr<PaymentStrategy> strategy_;  ///< Payment strategy
};

int main() {
    // Create an order with a credit card payment strategy
    Order order{49.99, std::make_unique<CreditCardPayment>()};
    order.Checkout();

    // Same Order object -> swap behavior at runtime, no recompilation
    order.SetPaymentStrategy(std::make_unique<PayPalPayment>());
    order.Checkout();

    // Set the payment strategy to crypto payment
    order.SetPaymentStrategy(std::make_unique<CryptoPayment>());
    order.Checkout();
}

/*
 * Note on cost (see strategy-to-policy-design-tutorial.md, Part 2): every
 * Pay() call here is a virtual dispatch through a vtable, and each strategy
 * object is a heap allocation. For payment methods, that's the right trade
 * -- the choice of payment method is a genuine runtime fact (the customer
 * picks it at checkout), unlike, say, a distance metric baked in at compile
 * time. That's why this stays a runtime Strategy rather than moving to the
 * compile-time policy-based version shown later in the tutorial.
 */
