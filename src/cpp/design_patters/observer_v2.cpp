#include <algorithm>
#include <iostream>
#include <vector>

// ------------------------------------------------------------------------------------------
// APPLIED VERSION — Observer provides a one-to-many notification relationship
// ------------------------------------------------------------------------------------------
//
// Simulation is the Subject: it owns the state that changes. Logger, Plotter, and
// Checkpointer are Observers: they subscribe to state-change notifications and
// decide independently how to react.
//
// Compared with observer_v1.cpp, Simulation depends only on one observer interface.
// New observer types can be introduced without modifying Simulation.
// ------------------------------------------------------------------------------------------

// Observer interface: every subscriber receives the same event contract
class SimulationObserver {
public:
    // Required when an observer is destroyed through this interface.
    virtual ~SimulationObserver() = default;
    virtual void OnStep(int step, double residual) = 0;
};

// Concrete observers implement independent reactions to the same event
class Logger : public SimulationObserver {
public:
    void OnStep(int step, double residual) override {
        std::cout << "[Logger] step " << step << ", residual = " << residual << "\n";
    }
};

class Plotter : public SimulationObserver {
public:
    void OnStep(int step, double residual) override {
        std::cout << "[Plotter] plotting point (" << step << ", " << residual << ")\n";
    }
};

class Checkpointer : public SimulationObserver {
public:
    void OnStep(int step, double residual) override {
        if (residual < 0.2) {
            std::cout << "[Checkpointer] residual below threshold -- writing checkpoint at step "
                      << step << "\n";
        }
    }
};

/*
 * Subject: owns simulation state and publishes a notification after each state
 * transition. It knows the SimulationObserver interface, but it does not depend on
 * any concrete observer type.
 *
 * The stored pointers are non-owning. Every attached observer must outlive either
 * its subscription or the Simulation object. References in Attach/Detach prevent
 * null subscriptions, while pointers in the vector make identity comparison and
 * removal straightforward.
 */
class Simulation {
public:
    // Attach at most once so one observer receives one callback per step
    void Attach(SimulationObserver& observer) {
        if (std::find(observers_.begin(), observers_.end(), &observer) == observers_.end()) {
            observers_.push_back(&observer);
        }
    }

    // Detaching an observer that is not subscribed has no effect
    void Detach(SimulationObserver& observer) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), &observer), observers_.end()
        );
    }

    void Step() {
        ++this->step_;
        this->residual_ *= 0.5;  // Simplified convergence model

        // Publish only after the new state is complete and internally consistent
        this->NotifyAll();
    }

private:
    int step_{0};
    double residual_{1.};
    std::vector<SimulationObserver*> observers_;

    void NotifyAll() {
        // Contract for this simple example: callbacks must not attach or detach
        // observers while this iteration is in progress.
        for (auto* observer : observers_) {
            observer->OnStep(this->step_, this->residual_);
        }
    }
};

int main() {
    Simulation sim;

    Logger logger;
    Plotter plotter;
    Checkpointer checkpointer;

    // The caller composes the subscriber set at runtime.
    sim.Attach(logger);
    sim.Attach(plotter);
    sim.Attach(checkpointer);

    sim.Step();
    sim.Step();

    // Subscription can change without modifying Simulation or the other observers.
    sim.Detach(plotter);
    std::cout << "-- plotter detached --\n";
    sim.Step();
}

/*
 * Important production considerations:
 *   - Raw observer pointers express non-ownership but require disciplined lifetime
 *     management. A connection object that automatically unsubscribes on
 *     destruction can provide safer RAII semantics.
 *   - If callbacks may modify subscriptions, NotifyAll() needs a defined policy,
 *     such as iterating over a snapshot or deferring modifications.
 *   - Exceptions, callback ordering, duplicate subscriptions, and thread safety are
 *     part of the Subject's public behavioral contract and should be specified.
 *   - Observer is justified when subscribers vary independently. For a small,
 *     permanent set of required collaborators, direct calls may remain clearer.
 */
