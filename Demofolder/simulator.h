#pragma once
#include <vector>

// Stores the main results of one misinformation spread simulation.
struct SimResult {
    int total;      // Total nodes infected
    int plateau;    // Last step with a new infection
    int peak;       // Maximum simultaneous infections
};

// Stores infection information at each simulation step.
struct TraceRow {
    int step;
    int susceptible;
    int infected;
    int recovered;
    int newInfected;
};

// Stores parameters used by the spread simulator.
struct SimulationConfig {
    double beta;    // Probability of misinformation spreading
    double gamma;   // Probability of recovery
}