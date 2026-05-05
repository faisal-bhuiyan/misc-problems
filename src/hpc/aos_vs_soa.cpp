/**
 * @file aos_vs_soa.cpp
 * @brief Benchmark: Array-of-Structures vs Structure-of-Arrays for a simple
 * particle update (apply gravity, integrate position).
 *
 * Build:
 *   clang++ -O3 -std=c++17 -march=native aos_vs_soa.cpp -o aos_vs_soa
 *   (on Apple Silicon, -march=native works with recent clang; if not,
 *    try -mcpu=apple-m1 or just drop the flag and rely on -O3)
 *
 * Try also:
 *   -O3 -fno-tree-vectorize           // disable auto-vectorization
 *   -O3 -Rpass=loop-vectorize         // ask clang what it vectorized
 *   -O3 -Rpass-missed=loop-vectorize  // ask clang what it didn't, and why
 */

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

constexpr std::size_t kNumParticles{1 << 22};  // ~4.2M particles
constexpr int kNumSteps{50};
constexpr float kStepSize{0.01f};
constexpr float kGravity{-9.81f};

//---------------------------------------------------------------------------
// Array of Structures
//---------------------------------------------------------------------------
struct Particle {
    float x, y, z;     // position
    float vx, vy, vz;  // velocity
    float mass;        // unused in math, here to make the struct realistic
    float pad;         // pad to 32 bytes to make alignment story honest
};

void update_aos(std::vector<Particle>& p, float kStepSize) {
    for (std::size_t i = 0; i < p.size(); ++i) {
        p[i].vy += kGravity * kStepSize;
        p[i].x += p[i].vx * kStepSize;
        p[i].y += p[i].vy * kStepSize;
        p[i].z += p[i].vz * kStepSize;
    }
}

//---------------------------------------------------------------------------
// Structure of Arrays
//---------------------------------------------------------------------------
struct ParticlesSoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;

    void resize(std::size_t n) {
        x.resize(n);
        y.resize(n);
        z.resize(n);
        vx.resize(n);
        vy.resize(n);
        vz.resize(n);
        mass.resize(n);
    }
    std::size_t size() const { return x.size(); }
};

void update_soa(ParticlesSoA& particles, float kStepSize) {
    const std::size_t n = particles.size();
    // Pull pointers out so the compiler doesn't reload them each iteration
    float* __restrict__ x = particles.x.data();
    float* __restrict__ y = particles.y.data();
    float* __restrict__ z = particles.z.data();
    float* __restrict__ vx = particles.vx.data();
    float* __restrict__ vy = particles.vy.data();
    float* __restrict__ vz = particles.vz.data();

    for (std::size_t i = 0; i < n; ++i) {
        vy[i] += kGravity * kStepSize;
        x[i] += vx[i] * kStepSize;
        y[i] += vy[i] * kStepSize;
        z[i] += vz[i] * kStepSize;
    }
}

//---------------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------------
template <class F>
double time_ms(F&& f) {
    auto t0 = std::chrono::steady_clock::now();
    f();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    std::cout << "Particles: " << kNumParticles << "  (" << std::fixed << std::setprecision(1)
              << (kNumParticles * sizeof(Particle)) / (1024. * 1024.) << " MB AoS, "
              << (kNumParticles * 7 * sizeof(float)) / (1024. * 1024.) << " MB SoA)\n";
    std::cout << "Steps per run: " << kNumSteps << "\n\n";

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.f, 1.f);

    // Initialize AoS
    std::vector<Particle> aos(kNumParticles);
    for (auto& p : aos) {
        p.x = dist(rng);
        p.y = dist(rng);
        p.z = dist(rng);
        p.vx = dist(rng);
        p.vy = dist(rng);
        p.vz = dist(rng);
        p.mass = 1.f;
        p.pad = 0.f;
    }

    // Initialize SoA with the same numbers, so results are comparable
    ParticlesSoA soa;
    soa.resize(kNumParticles);
    for (std::size_t i = 0; i < kNumParticles; ++i) {
        soa.x[i] = aos[i].x;
        soa.y[i] = aos[i].y;
        soa.z[i] = aos[i].z;
        soa.vx[i] = aos[i].vx;
        soa.vy[i] = aos[i].vy;
        soa.vz[i] = aos[i].vz;
        soa.mass[i] = 1.f;
    }

    // Warm up caches so we're not measuring cold misses
    update_aos(aos, kStepSize);
    update_soa(soa, kStepSize);

    // Benchmark AoS
    double aos_ms = time_ms([&] {
        for (int s = 0; s < kNumSteps; ++s) {
            update_aos(aos, kStepSize);
        }
    });

    // Benchmark SoA
    double soa_ms = time_ms([&] {
        for (int s = 0; s < kNumSteps; ++s) {
            update_soa(soa, kStepSize);
        }
    });

    // Sanity check: positions should agree closely (floating point order
    // can differ slightly, but this is the same math so they should match).
    double max_diff = 0.;
    for (std::size_t i = 0; i < kNumParticles; ++i) {
        max_diff = std::max(max_diff, (double)std::abs(aos[i].x - soa.x[i]));
        max_diff = std::max(max_diff, (double)std::abs(aos[i].y - soa.y[i]));
        max_diff = std::max(max_diff, (double)std::abs(aos[i].z - soa.z[i]));
    }

    std::cout << "AoS:  " << std::fixed << std::setprecision(2) << std::setw(8) << aos_ms
              << " ms total  (" << (aos_ms / kNumSteps) << " ms/step)\n";
    std::cout << "SoA:  " << std::setw(8) << soa_ms << " ms total  (" << (soa_ms / kNumSteps)
              << " ms/step)\n";
    std::cout << "Speedup (AoS/SoA): " << (aos_ms / soa_ms) << "x\n";
    std::cout << "Max position diff between layouts: " << std::defaultfloat << std::setprecision(3)
              << max_diff << '\n';

    // Throughput, in GB/s of memory touched per step
    // AoS: reads/writes the whole 32-byte struct per particle => 32 B touched
    // SoA: reads vx,vy,vz,x,y,z (some r/w), mass untouched => ~24 B / particle
    double aos_gb = (double)kNumParticles * 32. * kNumSteps / (aos_ms / 1000.) / 1e9;
    double soa_gb = (double)kNumParticles * 24. * kNumSteps / (soa_ms / 1000.) / 1e9;
    std::cout << "\nApprox effective bandwikStepSizeh:\n";
    std::cout << std::fixed << std::setprecision(1) << "  AoS: " << aos_gb << " GB/s\n";
    std::cout << "  SoA: " << soa_gb << " GB/s\n";

    return 0;
}
