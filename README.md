# High-Performance-Monte-Carlo-Engine
# monte-carlo-pricer

GBM-based Monte Carlo engine in C++20. Prices European-style payoffs and exports
sample paths for visualization.

Runs 10M simulations in ~60ms on an 8-core machine. Each thread gets its own RNG
so there's no locking anywhere in the hot path.

## Build

g++ -std=c++20 -O3 -march=native quant_engine.cpp -o quant_engine

## Usage

Just run the binary. It prints E[S(T)] and VaR95, then writes paths.csv.
Feed that into animate.py to get the visualization.

python animate.py

## Params

S0, mu, sigma, T are hardcoded at the top of main() — edit them directly.
CSV export is 300 paths × 252 steps (one trading year).

## Dependencies

C++20 (tested on GCC 13), Python 3 with matplotlib and pandas.
