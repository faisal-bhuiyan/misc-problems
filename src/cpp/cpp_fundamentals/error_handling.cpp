// error_handling.cpp: Faisal Bhuiyan
// Description: Illustrates the three error-reporting mechanisms and the role
//              of RAII in simple, efficient exception-based error handling

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

/*
 * In well-designed code, try-blocks are rare. This is not because errors
 * are ignored — it is because RAII (Resource Acquisition Is Initialization)
 * handles cleanup automatically, removing the need to catch exceptions just
 * to release resources.
 *
 * A function that cannot perform its allocated task has three options:
 *   1. Throw an exception              —> signals truly unexpected failure
 *   2. Return a value indicating error —> for expected, normal non-results
 *   3. Terminate the program           —> for unrecoverable failures only
 *
 * RAII is what makes exception-based error handling practical: destructors
 * run automatically during stack unwinding, so resources are released even
 * when an exception propagates across many call frames. Without RAII,
 * every frame between the throw and the catch becomes a potential leak.
 */

// ===============================================================================
// Resource: simulates any acquired resource — a file handle, network connection,
// or database cursor. Prints on acquire and release so we can observe its
// lifetime clearly.
// ===============================================================================
class Resource {
public:
    explicit Resource(const std::string& name) : name_{name}, open_{true} {
        std::cout << "    [acquired] " << name_ << "\n";
    }

    ~Resource() {
        if (open_) {
            release();
        }
    }

    void use() {
        if (!open_) {
            throw std::logic_error(name_ + ": use after release");
        }
        std::cout << "    [using]    " << name_ << "\n";
    }

    void release() {
        open_ = false;
        std::cout << "    [released] " << name_ << "\n";
    }

private:
    std::string name_;
    bool open_;
};

// ===============================================================================
// PART 1: THREE WAYS TO REPORT AN ERROR
//
// The same task — looking up a record by id — expressed three
// ways. The right choice depends on whether failure is
// unexpected, a normal outcome, or truly unrecoverable.
// ===============================================================================

struct Record {
    int id;
    std::string data;
};

// ------------------------------------------------------------------
// Method 1: throw an exception
// Use when: failure is unexpected and the caller cannot reasonably
// handle the absence inline. The call stack unwinds to whoever
// can handle it — the caller is not forced to check a return value.
// ------------------------------------------------------------------
Record find_record_throws(int id) {
    if (id != 42) {
        throw std::runtime_error("record not found: " + std::to_string(id));
    }
    return {42, "employee data"};
}

// ------------------------------------------------------------------
// Method 2: return a value indicating failure (std::optional)
// Use when: not finding a record is a normal, expected outcome.
// The caller is forced to check — absence is part of the interface.
// No exception overhead on the common path.
// ------------------------------------------------------------------
std::optional<Record> find_record_optional(int id) {
    if (id != 42) {
        return std::nullopt;
    }
    return Record{42, "employee data"};
}

// ------------------------------------------------------------------
// Method 3: terminate the program
// Use when: the program cannot safely continue — missing critical
// configuration, a violated fundamental invariant, or out-of-memory
// in an embedded system where no recovery path exists.
//
// std::terminate() is commented out below — if enabled, the program
// ends immediately: no unwinding, no destructors, no recovery.
// It is shown here to make it clear this is a deliberate design choice,
// not a fallback.
// ------------------------------------------------------------------
Record find_critical_config(int id) {
    if (id != 42) {
        std::cerr << "  FATAL: critical config record " << id << " missing — cannot continue.\n";
        // std::terminate();    // uncomment to see immediate program end

        // For demo purposes only — we simulate the outcome without
        // actually ending the program:
        throw std::runtime_error("simulating terminate for demo purposes");
    }
    return {42, "critical config"};
}

// ===============================================================================
// PART 2: WITHOUT RAII — try-blocks multiply
//
// Every function that acquires resources must catch exceptions
// just to release them, then re-throw. Three consequences:
//   - cleanup code is written twice (success path and catch path)
//   - adding a resource means updating two cleanup sites
//   - forgetting one site causes a leak
// ===============================================================================
void process_without_raii() {
    std::cout << "\n  acquiring resources manually...\n";
    Resource* buffer = new Resource{"raw buffer"};
    Resource* handle = new Resource{"raw file handle"};

    try {
        buffer->use();
        handle->use();
        throw std::runtime_error("something failed mid-process");
    } catch (...) {
        std::cout << "  exception caught — manually cleaning up\n";
        delete buffer;  // must remember every resource
        delete handle;  // must remember every resource
        throw;          // re-throw so the caller knows something failed
    }

    delete buffer;  // also required on the success path — written twice
    delete handle;
}

// ===============================================================================
// PART 3: WITH RAII — try-blocks disappear
//
// unique_ptr's destructor calls ~Resource() automatically,
// whether we exit by return or by exception. No manual cleanup,
// no duplicated delete calls, no risk of leaking on any path.
// ===============================================================================
void process_with_raii() {
    std::cout << "\n  acquiring resources via RAII wrappers...\n";
    auto buffer = std::make_unique<Resource>("managed buffer");
    auto handle = std::make_unique<Resource>("managed file handle");

    buffer->use();
    handle->use();
    throw std::runtime_error("something failed mid-process");

    // No try-catch here. If we return normally, destructors run.
    // If we throw, destructors still run — during stack unwinding.
    // The cleanup logic lives in exactly one place: the destructor.
}

// ===============================================================================
// PART 4: RAII + EXCEPTIONS AS A DESIGNED PAIR
//
// An exception thrown deep in the call stack propagates upward,
// unwinding each frame. RAII ensures every frame releases its
// resources as the stack unwinds — automatically, in reverse
// order of construction — regardless of how many frames deep
// the exception originates.
// ===============================================================================
void level_three() {
    auto r = std::make_unique<Resource>("level-3 resource");
    r->use();
    throw std::runtime_error("failure deep in the call stack");
}  // r released HERE — during unwinding of level_three's frame

void level_two() {
    auto r = std::make_unique<Resource>("level-2 resource");
    r->use();
    level_three();
}  // r released HERE — during unwinding of level_two's frame

void level_one() {
    auto r = std::make_unique<Resource>("level-1 resource");
    r->use();
    level_two();
}  // r released HERE — during unwinding of level_one's frame

int main() {
    // ----------------------------------------------------------
    // Part 1: three error reporting mechanisms
    // ----------------------------------------------------------
    std::cout << "\n=========== THREE ERROR REPORTING MECHANISMS ===========\n";

    std::cout << "\n--- Method 1: throw (unexpected failure) ---\n";
    try {
        auto r = find_record_throws(99);
    } catch (const std::runtime_error& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }

    std::cout << "\n--- Method 2: std::optional (expected non-result) ---\n";
    auto result = find_record_optional(99);
    if (result) {
        std::cout << "  found: " << result->data << "\n";
    } else {
        std::cout << "  not found — handled inline, no exception overhead\n";
    }

    std::cout << "\n--- Method 3: terminate (unrecoverable failure, simulated) ---\n";
    try {
        auto cfg = find_critical_config(99);
    } catch (const std::runtime_error& e) {
        std::cout << "  (terminate simulated): " << e.what() << "\n";
    }

    // ----------------------------------------------------------
    // Part 2: without RAII — manual cleanup, try-blocks required
    // ----------------------------------------------------------
    std::cout << "\n=========== WITHOUT RAII ===========\n";
    try {
        process_without_raii();
    } catch (const std::runtime_error& e) {
        std::cout << "  caller caught: " << e.what() << "\n";
    }

    // ----------------------------------------------------------
    // Part 3: with RAII — no try-blocks inside the function
    // ----------------------------------------------------------
    std::cout << "\n=========== WITH RAII ===========\n";
    try {
        process_with_raii();
    } catch (const std::runtime_error& e) {
        std::cout << "  caller caught: " << e.what() << "\n";
    }

    // ----------------------------------------------------------
    // Part 4: RAII + exceptions across multiple call frames —
    // exception thrown in level_three, caught here in main.
    // Every frame releases its resource automatically.
    // ----------------------------------------------------------
    std::cout << "\n=========== RAII + EXCEPTIONS ACROSS CALL FRAMES ===========\n";
    std::cout << "  (exception thrown in level_three — caught in main)\n\n";
    try {
        level_one();
    } catch (const std::runtime_error& e) {
        std::cout << "\n  caught in main: " << e.what() << "\n";
        std::cout << "  all three resources released automatically\n";
    }

    return 0;
}
