#include <iostream>

// ------------------------------------------------------------------------------------------
// NAIVE VERSION — the subject is coupled directly to every dependent component
// ------------------------------------------------------------------------------------------
//
// A simulation produces progress events that several independent components may
// consume. In this version, Simulation names those components explicitly and invokes
// a different method on each one.
//
// Compare with observer_v2.cpp, where subscribers share one Observer interface and
// can be attached or detached without changing Simulation.
// ------------------------------------------------------------------------------------------

// Components interested in simulation progress. They have no common notification
// interface in this version.
class Logger {
public:
    void Log(int step, double residual) {
        std::cout << "[Logger] step " << step << ", residual = " << residual << "\n";
    }
};

class Plotter {
public:
    void UpdatePlot(int step, double residual) {
        std::cout << "[Plotter] plotting point (" << step << ", " << residual << ")\n";
    }
};

/*
 * Simulation is responsible for both advancing its state and coordinating every
 * component interested in that state.
 *
 * Consequences:
 *   - Simulation depends on the concrete Logger and Plotter types.
 *   - Its constructor fixes the subscriber set for the object's lifetime.
 *   - Adding a Checkpointer requires modifying the constructor, data members, and
 *     Step().
 *   - Each dependent exposes a different callback name, so notification cannot be
 *     expressed as one uniform operation.
 *
 * This direct design may still be appropriate when the dependent set is small,
 * fixed, and part of Simulation's core contract. Observer becomes useful when the
 * subscriber set must vary independently or at runtime.
 */
class Simulation {
public:
    Simulation(Logger& logger, Plotter& plotter) : logger_{logger}, plotter_{plotter} {}

    void Step() {
        ++this->step_;
        this->residual_ *= 0.5;  // Simplified convergence model

        // Notification logic is embedded directly in the state transition
        this->logger_.Log(this->step_, this->residual_);
        this->plotter_.UpdatePlot(this->step_, this->residual_);
    }

private:
    int step_{0};
    double residual_{1.};
    Logger& logger_;
    Plotter& plotter_;
};

int main() {
    Logger logger;
    Plotter plotter;
    Simulation sim{logger, plotter};

    sim.Step();
    sim.Step();
    sim.Step();
}

/*
 * The Observer pattern extracts the notification contract from these concrete
 * dependents. The subject then publishes a state change to an abstract subscriber
 * set instead of naming each reaction directly.
 */
