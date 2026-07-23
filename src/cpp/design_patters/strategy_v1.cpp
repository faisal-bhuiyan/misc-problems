#include <iostream>
#include <string>

// ------------------------------------------------------------------------------------------
// NAIVE VERSION — "the algorithm that varies" is embedded directly into Order
// ------------------------------------------------------------------------------------------
//
// This is the starting point for the Strategy pattern tutorial: a class whose
// core responsibility (checking out an order) is entangled with a second,
// independent responsibility (how a specific payment method works). Watch
// for the pain points below as this file is compared against
// strategy_pattern_applied.cpp.
// ------------------------------------------------------------------------------------------

class Order {
public:
    explicit Order(double amount) : amount_{amount} {}

    // Checkout() has two jobs smashed into one function:
    //   1. "Charge the customer" (Order's actual job)
    //   2. "Know the mechanics of every payment method" (not Order's job)
    //
    // Pain points this creates:
    //   - Open/Closed violation: adding a new payment method (e.g. "apple_pay")
    //     means editing Order itself, not extending it.
    //   - The if/else chain grows without bound as payment types are added,
    //     and every branch's logic lives in the same function -- no isolation.
    //   - payment_type is a bare string: no compile-time safety, easy to
    //     typo, and the "unknown payment type" fallback is a silent runtime
    //     failure instead of a compile error.
    //   - Order can't be unit-tested independently of every payment method's
    //     real logic (e.g. real card validation, real OAuth calls) -- there's
    //     no seam to substitute a fake/mock strategy.
    //   - Order and "how PayPal works" are compiled, versioned, and
    //     understood together, even though they change for unrelated reasons.
    void Checkout(const std::string& payment_type) {
        if (payment_type == "credit_card") {
            std::cout << "Charging $" << amount_ << " to credit card...\n";
            // credit-card-specific logic: validate card number, call card network...
        } else if (payment_type == "paypal") {
            std::cout << "Redirecting to PayPal for $" << amount_ << "...\n";
            // paypal-specific logic: OAuth redirect, handle callback...
        } else if (payment_type == "crypto") {
            std::cout << "Generating wallet address for $" << amount_ << "...\n";
            // crypto-specific logic: generate address, wait for confirmations...
        } else {
            std::cout << "Unknown payment type\n";
        }
    }

private:
    double amount_;
};

/*
 * See strategy_pattern_applied.cpp: the fix is to extract "how a payment
 * happens" into its own PaymentStrategy interface, so Order depends on an
 * abstraction (Pay()) instead of a growing if/else ladder of concrete
 * payment mechanics.
 */
int main() {
    Order order{49.99};
    order.Checkout("credit_card");
    order.Checkout("paypal");
    order.Checkout("crypto");
}
