#ifndef RUMORGUARD_SPREAD_SIMULATOR_H
#define RUMORGUARD_SPREAD_SIMULATOR_H

#include "../graph/Graph.h"
#include <vector>
#include <unordered_set>

/**
 * @struct SimulationStepLog
 * @brief Records metrics at each discrete time step of spread.
 */
struct SimulationStepLog {
    int step;
    int newlyInfectedCount;
    int totalInfected;
    int activeInfectedCount;
    int recoveredCount;
};

/**
 * @struct SimulationResult
 * @brief Summary outcome of a single simulation trial.
 */
struct SimulationResult {
    int totalInfected;
    int stepsToPlateau;
    std::vector<SimulationStepLog> stepLogs;
};

/**
 * @class SpreadSimulator
 * @brief Implements an SIR-style probabilistic rumor contagion model on graphs.
 *
 * Algorithmic Justification:
 * - SIR (Susceptible-Infectious-Recovered) dynamic models rumor spreading realistically:
 *   1. Susceptible (S): Vulnerable to believing & sharing unverified rumors.
 *   2. Infectious (I): Actively spreading the rumor to connected neighbors.
 *   3. Recovered/Fact-Checked (R): Realized misinformation or stopped sharing after a period.
 *   4. Vaccinated (V): Immunized via pre-emptive fact-checking / verification tags.
 * - Single-pass BFS-style iteration across active frontier nodes per timestep keeps
 *   each simulation step O(E_active) in time.
 */
class SpreadSimulator {
public:
    int recoverySteps;       // Timesteps an infected node remains active before becoming recovered
    int maxSteps;            // Safety upper bound on simulation duration
    int plateauThreshold;    // Number of consecutive zero-infection steps to trigger plateau stop
    double baseTransmission; // Base probability factor for spread

    SpreadSimulator(int recovery = 5, int maxStep = 100, int plateau = 3, double baseProb = 0.45)
        : recoverySteps(recovery), maxSteps(maxStep),
          plateauThreshold(plateau), baseTransmission(baseProb) {}

    /**
     * Runs a stochastic simulation trial on the graph with a specified set of vaccinated/targeted nodes.
     * @param graph The network (a copy or clean-state graph)
     * @param vaccinatedNodes Node IDs targeted for immunization
     * @param trialSeed PRNG seed for deterministic evaluation runs
     * @return SimulationResult containing total infections, steps to plateau, and step-by-step log
     */
    SimulationResult runSimulation(Graph& graph, const std::vector<int>& vaccinatedNodes, unsigned int trialSeed = 12345);
};

#endif // RUMORGUARD_SPREAD_SIMULATOR_H
