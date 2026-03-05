/**
 * @file quant_engine.cpp
 * @author Mostafa Elzoghby
 *
 * Monte Carlo pricer — GBM, European terminal payoff only.
 * Exports a CSV of sample paths for the animation script.
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fstream>

struct SimulationResults {
    double mean_price;
    double var_95;
    long long duration_ms;
};

void simulate_batch(int n, double S0, double mu, double sigma, double T, std::vector<double>& out) {
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> ndist(0.0, 1.0);

    double drift     = (mu - 0.5 * sigma * sigma) * T;
    double diffusion = sigma * std::sqrt(T);

    for (int i = 0; i < n; ++i)
        out[i] = S0 * std::exp(drift + diffusion * ndist(gen));
}

// writes rows=paths, cols=timesteps — that's what the python script expects
void write_paths_csv(const std::string& fname, int n_paths, int steps,
                     double S0, double mu, double sigma, double T) {
    std::ofstream f(fname);

    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> ndist(0.0, 1.0);

    double dt        = T / steps;
    double drift     = (mu - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);

    for (int i = 0; i < n_paths; ++i) {
        double S = S0;
        f << S;
        for (int t = 1; t <= steps; ++t) {
            S *= std::exp(drift + diffusion * ndist(gen));
            f << ',' << S;
        }
        f << '\n';
    }
}

int main() {
    const int TOTAL_SIMS = 10'000'000;
    const int NTHREADS   = static_cast<int>(std::thread::hardware_concurrency());
    const int SIMS_EACH  = TOTAL_SIMS / NTHREADS;

    double S0    = 100.0;
    double mu    = 0.05;
    double sigma = 0.25;
    double T     = 1.0;

    std::cout << "threads: " << NTHREADS << "  paths: " << TOTAL_SIMS << '\n';

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<double>> buckets(NTHREADS, std::vector<double>(SIMS_EACH));
    std::vector<std::jthread> workers;
    workers.reserve(NTHREADS);

    for (int i = 0; i < NTHREADS; ++i)
        workers.emplace_back(simulate_batch, SIMS_EACH, S0, mu, sigma, T, std::ref(buckets[i]));

    workers.clear(); // jthread joins on destruction

    auto t1 = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::vector<double> prices;
    prices.reserve(TOTAL_SIMS);
    for (auto& b : buckets)
        prices.insert(prices.end(), b.begin(), b.end());

    double mean = std::accumulate(prices.begin(), prices.end(), 0.0) / TOTAL_SIMS;

    // 5th percentile — nth_element is O(n) so no reason not to use it
    std::vector<double> tmp = prices;
    int idx = TOTAL_SIMS / 20;
    std::nth_element(tmp.begin(), tmp.begin() + idx, tmp.end());
    double var_95 = tmp[idx];

    std::cout << "\ntime       : " << ms << " ms\n";
    std::cout << "throughput : " << std::fixed << std::setprecision(2)
              << (TOTAL_SIMS / (ms / 1000.0)) / 1e6 << " M paths/sec\n";
    std::cout << "E[S(T)]    : $" << std::setprecision(4) << mean << '\n';
    std::cout << "VaR 95%    : $" << var_95 << '\n';

    // dump paths for the animation — 300 paths, 252 steps (trading days)
    write_paths_csv("paths.csv", 300, 252, S0, mu, sigma, T);
    std::cout << "paths.csv written.\n";

    return 0;
}