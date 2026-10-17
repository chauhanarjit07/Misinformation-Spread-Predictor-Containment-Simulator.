# RumorGuard — Graph-Based Misinformation Spread & Containment Simulator

A C++17 data structures and object-oriented programming project for simulating misinformation spread and evaluating containment strategies.

## Project Purpose

RumorGuard models social networks as graphs using Barabási–Albert preferential attachment (scale-free networks), computes key topological features (clustering coefficient, betweenness centrality via Brandes' algorithm), and runs stochastic SIR-style infection simulations. Four containment strategies (No Intervention, Degree-Based, Centrality-Based, Greedy Influence) are evaluated at budgets of 5%, 10%, and 20%.

## Build Instructions

```bash
mkdir build && cd build && cmake .. && make
```

Requires CMake and a C++17 compiler (g++ or clang++).

## Running the Simulation

```bash
./rumorguard --nodes 500 --m 3 --trials 20 --outdir results/
```

Arguments:
- `--nodes`: Total nodes in the generated graph
- `--m`: Number of edges per new node (Barabási–Albert parameter)
- `--trials`: Number of simulation runs per strategy/budget combination
- `--outdir`: Directory for output CSV files

## UI (Visualization Layer)

Open `ui/index.html` in any modern web browser. It reads `results/comparison_results.csv` and `results/simulation_log.csv` (via file picker or paste) and displays interactive charts and tables. The UI is a separate visualization layer; the C++ engine is the authoritative simulation.

## Module Design Justifications

- **Graph**: `std::unordered_map` adjacency list provides O(1) average lookup for sparse graphs.
- **Preferential Attachment**: Produces realistic scale-free networks matching social media structures.
- **Brandes' Algorithm**: O(V·(V+E)) complexity, far faster than naive all-pairs shortest paths (O(V³)).
- **BFS Pass**: Used for infection spread (single pass over adjacency list) because the graph is unweighted and BFS ensures each node is processed exactly once per step.
- **Greedy Strategy**: Approximation for the NP-hard influence maximization problem (Kempe et al., 2003).

## Source Tree

```
src/
  graph/        - Graph, Barabási–Albert generation
  features/     - Degree, clustering coefficient, betweenness centrality
  simulation/   - SpreadSimulator (SIR-style probabilistic model)
  strategies/   - Containment strategies (NoIntervention, Degree, Centrality, Greedy)
  evaluation/   - main.cpp (CLI pipeline) and CSV output
```
