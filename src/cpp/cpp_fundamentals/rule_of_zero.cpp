// rule_of_zero.cpp: Faisal Bhuiyan
// Description: The Rule of Zero — define all essential operations or none.
//              Demonstrates = default, = delete, and explicit constructors.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*
 * THE RULE OF ZERO / FIVE:
 *
 * If a class manages a raw resource (owning raw pointer, file descriptor,
 * GPU handle), define ALL five essential operations:
 *   1. Destructor
 *   2. Copy constructor
 *   3. Copy assignment
 *   4. Move constructor
 *   5. Move assignment
 *
 * If a class uses only well-behaved RAII members (std::string, std::vector,
 * std::unique_ptr), define NONE — the compiler generates correct versions
 * by composing the members' own operations. This is the Rule of Zero.
 *
 * The danger of defining some but not all: if you define a destructor, the
 * compiler still generates copy operations — but they do a shallow memberwise
 * copy of raw pointers. Two objects then believe they own the same memory.
 * The destructor fires twice: undefined behaviour.
 *
 * = default: explicitly request the compiler-generated version. Communicates
 *            intent and can restore a defaulted operation that would otherwise
 *            be suppressed (e.g. default ctor after declaring another ctor).
 *
 * = delete: permanently remove an operation. Misuse becomes a compile error,
 *           not a runtime crash or silent wrong result.
 *
 * explicit: prevent single-argument constructors from acting as implicit
 *           conversions. Silence from the compiler is not agreement.
 */

// ====================================================================
// PART 1: RULE OF ZERO
//
// Employee uses only RAII members (std::string, std::vector).
// No special members are defined — the compiler composes correct
// copy, move, and destructor from the members' own operations.
// Adding a member never breaks the invariant: it handles itself.
// ====================================================================
class Employee {
public:
    Employee(std::string name, int id) : name_{std::move(name)}, id_{id} {
        std::cout << "    [ctor]  Employee(" << this->name_ << ", " << this->id_ << ")\n";
    }

    void AddProject(const std::string& p) { this->projects_.push_back(p); }

    void Print() const {
        std::cout << "    Employee name: " << this->name_ << " id: " << this->id_
                  << " projects count: " << this->projects_.size() << "\n";
    }

    // No ~Employee, no copy/move — compiler generates all five correctly.
    // std::string and std::vector handle their own memory.

private:
    std::string name_;                   // RAII member
    int id_;                             // RAII member
    std::vector<std::string> projects_;  // RAII member
};

// =================================================================================
// PART 2: RULE OF FIVE (contrast with Rule of Zero)
//
// RawBuffer manages a raw heap pointer — it MUST define all five.
// Omitting any one leads to bugs:
//   - no copy ctor:   compiler shallow-copies the pointer -> double free
//   - no copy assign: same problem with operator=
//   - no move ctor:   copy used instead -> expensive O(n) where O(1) suffices
// =================================================================================
class RawBuffer {
public:
    explicit RawBuffer(int size) : data_{new double[size]{}}, size_{size} {
        std::cout << "    [ctor]       RawBuffer(" << this->size_ << ")\n";
    }

    // 1. Destructor
    ~RawBuffer() {
        std::cout << "    [dtor]       RawBuffer(" << this->size_ << ")\n";
        delete[] data_;
    }

    // 2. Copy constructor — deep copy: allocate + copy all elements O(n)
    RawBuffer(const RawBuffer& other) : data_{new double[other.size_]}, size_{other.size_} {
        // Copy data from other to this
        for (int i = 0; i < this->size_; ++i) {
            this->data_[i] = other.data_[i];
        }
        std::cout << "    [copy ctor]  RawBuffer(" << this->size_ << ") — copied " << this->size_
                  << " doubles\n";
    }

    // 3. Copy assignment
    RawBuffer& operator=(const RawBuffer& other) {
        // Check for self-assignment
        if (this == &other) {
            return *this;
        }

        // Release the old resource
        delete[] this->data_;

        // Allocate the new resource
        this->data_ = new double[other.size_];

        // Copy the data
        this->size_ = other.size_;
        for (int i = 0; i < this->size_; ++i) {
            this->data_[i] = other.data_[i];
        }
        std::cout << "    [copy assign] RawBuffer(" << this->size_ << ")\n";
        return *this;
    }

    // 4. Move constructor — steal pointer: O(1)
    RawBuffer(RawBuffer&& other) noexcept : data_{other.data_}, size_{other.size_} {
        // Leave the source in a valid but empty state
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "    [move ctor]  RawBuffer(" << this->size_ << ") — stole pointer\n";
    }

    // 5. Move assignment
    RawBuffer& operator=(RawBuffer&& other) noexcept {
        // Check for self-assignment
        if (this == &other) {
            return *this;
        }

        // Release the old resource
        delete[] this->data_;

        // Transfer ownership of the resource
        this->data_ = other.data_;
        this->size_ = other.size_;

        // Leave the source in a valid but empty state
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "    [move assign] RawBuffer(" << this->size_ << ")\n";
        return *this;
    }

    int Size() const { return this->size_; }

    void Print() const {
        std::cout << "    RawBuffer: size=" << this->Size() << " data_@" << (void*)this->data_
                  << "\n";
    }

private:
    double* data_;  // resource handle
    int size_;
};

// ====================================================================
// PART 3: = default
//
// Two important use cases:
//
// A) Restoring the default constructor.
//    Declaring ANY constructor suppresses the compiler-generated
//    default constructor. = default restores it explicitly.
//
// B) Making intent visible.
//    = default documents: "the compiler's version is intentionally
//    correct here — this is not an oversight."
// ====================================================================
class Config {
public:
    Config() = default;  // A) restored: declaring Config(path) below
                         //    would otherwise suppress Config()

    explicit Config(const std::string& path) : path_{path} {
        std::cout << "    [ctor]  Config(\"" << this->path_ << "\")\n";
    }

    Config(const Config&) = default;  // B) intent explicit: memberwise copy is correct
    Config& operator=(const Config&) = default;
    Config(Config&&) = default;
    Config& operator=(Config&&) = default;
    ~Config() = default;

    void Print() const {
        std::cout << "    Config: path=\"" << this->path_ << "\"  debug=" << this->debug_ << "\n";
    }

private:
    std::string path_;   // RAII member
    bool debug_{false};  // default initialized RAII member
};

// ====================================================================
// PART 4: = delete  —  three distinct use cases
//
// A) Move-only type: owns a resource that cannot be duplicated
//    (file handle, GPU context, network socket).
//    Copy is deleted — move is allowed.
//
// B) Preventing implicit numeric conversions at a call site.
//    Without deletion, set_threshold(42) silently converts int to
//    double — potentially wrong units or precision. Deletion forces
//    the caller to be explicit.
//
// C) Forcing non-default construction.
//    Deleting the default constructor ensures the object is always
//    constructed with meaningful initial values.
// ====================================================================

// A) Move-only handle
class FileHandle {
public:
    explicit FileHandle(const std::string& path) : path_{path} {
        std::cout << "    [ctor]  FileHandle(\"" << this->path_ << "\")\n";
    }
    ~FileHandle() { std::cout << "    [dtor]  FileHandle(\"" << this->path_ << "\")\n"; }

    FileHandle(const FileHandle&) = delete;  // cannot duplicate a file handle
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&&) noexcept = default;  // ownership transfer is fine
    FileHandle& operator=(FileHandle&&) noexcept = default;

    const std::string& Path() const { return this->path_; }

private:
    std::string path_;  // RAII member
};

// B) Preventing implicit conversions
class Sensor {
public:
    explicit Sensor(int channel) : channel_{channel} {}

    // Accepts only double — int and float overloads are deleted.
    // Without deletion: set_threshold(100) silently converts to 100.0
    // — correct here by coincidence, but masking a unit/precision mistake.
    void set_threshold(double v) { this->threshold_ = v; }
    void set_threshold(int) = delete;    // ❌ must convert explicitly: (double)100
    void set_threshold(float) = delete;  // ❌ same — no silent precision loss

    void Print() const {
        std::cout << "    Sensor[" << this->channel_ << "]  threshold=" << this->threshold_ << "\n";
    }

private:
    int channel_;           // RAII member
    double threshold_{0.};  // default initialized RAII member
};

// C) Forcing non-default construction (no valid "empty" state)
class Connection {
public:
    Connection() = delete;  // a Connection without a host makes no sense

    explicit Connection(const std::string& host, int port) : host_{host}, port_{port} {
        std::cout << "    [ctor]  Connection(" << this->host_ << ":" << this->port_ << ")\n";
    }

    void Print() const {
        std::cout << "    Connection: " << this->host_ << ":" << this->port_ << "\n";
    }

private:
    std::string host_;  // RAII member
    int port_;          // RAII member
};

// ====================================================================
// PART 5: explicit
//
// A single-argument constructor defines both construction and an
// implicit conversion. The conversion is almost never intended.
//
// Without explicit:
//   Matrix m = 5;       — silently creates a 5x5 matrix
//   process(3);         — silently creates a 3x3 matrix as argument
//   if (m == 5) ...     — silently creates Matrix(5) for comparison
//
// With explicit: all of the above become compile errors.
// Direct construction Matrix{5} still works — explicit blocks only
// implicit conversion, not intentional construction.
//
// Also applies to constructors with defaulted arguments that can be
// called with a single argument — same silent-conversion risk.
// ====================================================================
class Matrix {
public:
    // Without explicit, Matrix m = 5 would silently compile.
    explicit Matrix(int size) : data_(static_cast<size_t>(size * size), 0.), size_{size} {
        std::cout << "    [ctor]  Matrix(" << this->size_ << "x" << this->size_ << ")\n";
    }

    // Two-arg ctor with default — can be called as Matrix(rows).
    // explicit prevents Matrix m = 4 even here.
    explicit Matrix(int rows, int cols) : data_(static_cast<size_t>(rows * cols), 0.), size_{rows} {
        std::cout << "    [ctor]  Matrix(" << rows << "x" << cols << ")\n";
    }

    int Size() const { return this->size_; }

private:
    std::vector<double> data_;  // RAII member
    int size_;                  // RAII member
};

void ProcessMatrix(const Matrix& m) {
    std::cout << "    process_matrix: size=" << m.Size() << "\n";
}

int main() {
    // ---------------------------------------------------------------------
    // Part 1: Rule of Zero — no special members, compiler handles all
    // --------------------------------------------------------------------
    std::cout << "\n=========== RULE OF ZERO ===========\n";
    {
        Employee e1{"Alice", 1};
        e1.AddProject("Simulation");
        e1.AddProject("Visualisation");
        // e1 contains: Alice #1, projects: 2
        e1.Print();

        std::cout << "\n  copy construction:\n";
        Employee e2 = e1;  // compiler-generated copy -> correct
        e2.AddProject("Extra");

        // e1 contains: Alice #1, projects: 2
        e1.Print();  // unchanged -> e2 is a genuine deep copy
        // e2 contains: Alice #1, projects: 3
        e2.Print();

        std::cout << "\n  move construction:\n";
        Employee e3 = std::move(e2);  // compiler-generated move —> correct
        // e3 contains: Alice #1, projects: 3
        e3.Print();
        // e2 contains: empty -> moved-from state
        e2.Print();
    }

    // ----------------------------------------------------------
    // Part 2: Rule of Five — raw resource requires all five
    // ----------------------------------------------------------
    std::cout << "\n=========== RULE OF FIVE (contrast) ===========\n";
    {
        RawBuffer b1{8};
        // RawBuffer b1{8};  // size: 8 data_@0x555555555555
        std::cout << "  b1: ";
        b1.Print();

        std::cout << "\n  copy:\n";
        RawBuffer b2 = b1;  // deep copy — b1 unchanged
        // RawBuffer b2 = b1;  // size: 8 data_@0x555555555555
        std::cout << "  b1 after copy: ";
        b1.Print();
        std::cout << "  b2 after copy: ";
        b2.Print();
        std::cout << "  b1 after copy size: " << b1.Size() << "  b2 after copy size: " << b2.Size()
                  << "\n";

        std::cout << "\n  move:\n";
        RawBuffer b3 = std::move(b1);  // O(1) steal — b1 emptied
        // RawBuffer b3 = std::move(b1);  // size: 0 data_@0x0
        std::cout << "  b3 after move: ";
        b3.Print();
        std::cout << "  b1 after move size: " << b1.Size() << "  b3 after move size: " << b3.Size()
                  << "\n";
    }

    // ----------------------------------------------------------
    // Part 3: = default
    // ----------------------------------------------------------
    std::cout << "\n=========== = default ===========\n";
    {
        Config c1;                      // default ctor — restored with = default
        Config c2{"config/prod.json"};  // custom ctor
        Config c3 = c2;                 // copy — explicitly defaulted
        c1.Print();
        c2.Print();
        c3.Print();
    }

    // ----------------------------------------------------------
    // Part 4: = delete
    // ----------------------------------------------------------
    std::cout << "\n=========== = delete ===========\n";

    // A) Move-only
    std::cout << "\n  A) move-only FileHandle:\n";
    {
        FileHandle f1{"data.csv"};
        FileHandle f2 = std::move(f1);  // ✅ move — transfers ownership
        // FileHandle f3 = f2;           // ❌ deleted — won't compile
        std::cout << "  f2 owns: " << f2.Path() << "\n";
    }

    // B) Preventing implicit conversion
    std::cout << "\n  B) Sensor — deleted int/float overloads:\n";
    {
        Sensor s{3};
        s.set_threshold(0.75);  // ✅ double — accepted
        // s.set_threshold(1);           // ❌ deleted — int not accepted silently
        // s.set_threshold(0.5f);        // ❌ deleted — float not accepted silently
        s.Print();
    }

    // C) Deleted default constructor
    std::cout << "\n  C) Connection — no default construction:\n";
    {
        Connection conn{"localhost", 8080};  // ✅ must supply host + port
        // Connection bad;                  // ❌ deleted — no valid empty state
        conn.Print();
    }

    // ----------------------------------------------------------
    // Part 5: explicit
    // ----------------------------------------------------------
    std::cout << "\n=========== explicit ===========\n";
    {
        Matrix m1{5};     // ✅ direct construction — always works
        Matrix m2{3, 4};  // ✅ direct construction — rows x cols

        // Matrix m3 = 5;      // ❌ implicit conversion blocked by explicit
        // ProcessMatrix(5);  // ❌ implicit conversion blocked by explicit

        ProcessMatrix(m1);         // ✅ pass existing Matrix
        ProcessMatrix(Matrix{5});  // ✅ explicit temporary — intent is clear
    }

    return 0;
}
