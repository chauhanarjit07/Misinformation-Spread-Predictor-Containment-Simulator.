# 🛡️ RumorGuard

<div align="center">

### Misinformation Spread & Containment Simulator

A C++ graph-based simulation for studying how information propagates through a synthetic social network and how different containment strategies change the simulated spread.

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Graph Algorithms](https://img.shields.io/badge/Graph%20Algorithms-4B5563?style=for-the-badge)
![Simulation](https://img.shields.io/badge/Simulation-7C3AED?style=for-the-badge)

</div>

---

## 📖 Overview

**RumorGuard** models misinformation spreading through a synthetic social network generated using the **Barabási–Albert model**.

The simulator evaluates multiple containment strategies under a constrained intervention budget and compares their effect on the simulated spread.

> **Note:** This is an algorithmic simulation project. It does **not** use machine learning and does not predict real-world misinformation events.

## 🔬 Strategies Simulated

- No intervention
- Degree-based targeting
- Betweenness-centrality targeting
- Greedy influence maximization

These strategies are evaluated within the simulation framework.

## 🧠 Algorithms & Data Structures

- Adjacency lists
- BFS / DFS
- Priority queues
- Degree-based graph analysis
- Brandes' algorithm for betweenness centrality
- Graph traversal and influence simulation

## 🔄 Simulation Pipeline

```text
Synthetic Network
       │
       ▼
Seed Information Spread
       │
       ▼
Choose Containment Strategy
       │
       ▼
Apply Intervention Budget
       │
       ▼
Simulate Propagation
       │
       ▼
Measure Spread
       │
       ▼
Compare Simulation Results
```

## 📷 Project Preview

<p align="center">
  <img src="https://github.com/user-attachments/assets/3f3a0b67-8d07-4401-9746-e59171bf0693" alt="RumorGuard simulation interface" width="810" />
</p>

## 🛠️ Technology

| Area | Details |
|---|---|
| Language | C++ |
| Network Model | Barabási–Albert synthetic graph |
| Core Structures | Graphs, adjacency lists, priority queues |
| Algorithms | BFS, DFS, Brandes' algorithm |
| Approach | Simulation and comparative analysis |
| Machine Learning | Not used |

## 🎯 Project Goals

- Practice graph algorithms on a non-trivial problem
- Explore network-generation models
- Implement centrality and influence-based techniques
- Understand how intervention budgets affect simulations
- Build a reproducible algorithmic experiment

## 🚀 Getting Started

```bash
git clone https://github.com/chauhanarjit07/Misinformation-Spread-Predictor-Containment-Simulator..git
cd Misinformation-Spread-Predictor-Containment-Simulator.
```

Use a C++17-compatible compiler to build the relevant source files.

## 📚 Learning Outcomes

- Graph representation
- Graph traversal
- Priority queues
- Centrality algorithms
- Simulation design
- Complexity-aware problem solving
- C++ implementation of graph algorithms

## 👨‍💻 Author

**Arjit Chauhan**  
B.Tech Computer Science Student • C++ • DSA

---

<div align="center">

**Model → Simulate → Measure → Analyze → Improve.**

</div>
