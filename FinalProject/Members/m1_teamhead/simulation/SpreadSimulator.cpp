#include "SpreadSimulator.h"
#include <random>
#include <algorithm>
#include <unordered_set>

SimulationResult SpreadSimulator::runSimulation(Graph& graph, const std::vector<int>& vaccinatedNodes, unsigned int trialSeed) {
    SimulationResult result;
    result.totalInfected = 0;
    result.stepsToPlateau = 0;

    // 1. Reset all nodes to baseline susceptible state
    graph.resetStates();

    // 2. Apply immunization / vaccination
    std::unordered_set<int> vaccinatedSet(vaccinatedNodes.begin(), vaccinatedNodes.end());
    for (int vId : vaccinatedNodes) {
        if (graph.hasNode(vId)) {
            graph.getNode(vId).isVaccinated = true;
        }
    }

    std::mt19937 rng(trialSeed);
    std::uniform_real_distribution<double> uniformProb(0.0, 1.0);

    // 3. Select 1 to 3 initial seed nodes from susceptible candidates
    std::vector<int> candidates;
    for (const auto& pair : graph.nodes) {
        if (!pair.second.isVaccinated) {
            candidates.push_back(pair.first);
        }
    }

    if (candidates.empty()) {
        // Entire network vaccinated
        result.totalInfected = 0;
        result.stepsToPlateau = 0;
        return result;
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    int seedCount = std::min(static_cast<int>(candidates.size()), 3);
    seedCount = std::max(1, seedCount);

    std::unordered_set<int> activeInfected;
    std::unordered_set<int> allInfected;
    std::unordered_set<int> recovered;

    for (int i = 0; i < seedCount; ++i) {
        int seedId = candidates[i];
        Node& node = graph.getNode(seedId);
        node.isInfected = true;
        node.infectedAtStep = 0;
        activeInfected.insert(seedId);
        allInfected.insert(seedId);
    }

    result.totalInfected = static_cast<int>(allInfected.size());

    // Record initial step 0
    SimulationStepLog step0;
    step0.step = 0;
    step0.newlyInfectedCount = seedCount;
    step0.totalInfected = seedCount;
    step0.activeInfectedCount = seedCount;
    step0.recoveredCount = 0;
    result.stepLogs.push_back(step0);

    int consecutiveZeroInfections = 0;
    int currentStep = 1;

    // 4. Time-step simulation loop
    while (currentStep <= maxSteps && !activeInfected.empty()) {
        std::vector<int> newInfectionsThisStep;
        std::vector<int> newlyRecovered;

        // BFS-style iteration across active spreader frontier
        for (int infId : activeInfected) {
            Node& infNode = graph.getNode(infId);

            // Check if node transitions to recovered state
            if (currentStep - infNode.infectedAtStep >= recoverySteps) {
                newlyRecovered.push_back(infId);
                continue;
            }

            // Attempt transmission to each neighbor
            const std::vector<int>& neighbors = graph.getNeighbors(infId);
            for (int nbrId : neighbors) {
                Node& nbrNode = graph.getNode(nbrId);

                // Susceptible check: must not be vaccinated, infected, or recovered
                if (nbrNode.isVaccinated || nbrNode.isInfected || recovered.find(nbrId) != recovered.end()) {
                    continue;
                }

                // Probability model:
                // - Higher trustScore decreases transmission probability (higher media literacy / skepticism)
                // - Higher activityLevel increases exposure and transmission probability
                double susceptibility = (1.0 - 0.65 * nbrNode.trustScore);
                double exposure = (0.35 + 0.65 * nbrNode.activityLevel);
                double transmitProb = baseTransmission * susceptibility * exposure;

                // Clamp to realistic bounds [0.05, 0.95]
                transmitProb = std::max(0.05, std::min(0.95, transmitProb));

                if (uniformProb(rng) < transmitProb) {
                    nbrNode.isInfected = true;
                    nbrNode.infectedAtStep = currentStep;
                    newInfectionsThisStep.push_back(nbrId);
                }
            }
        }

        // Process recoveries
        for (int recId : newlyRecovered) {
            activeInfected.erase(recId);
            recovered.insert(recId);
            graph.getNode(recId).isInfected = false;
        }

        // Process new infections
        for (int newId : newInfectionsThisStep) {
            if (allInfected.find(newId) == allInfected.end()) {
                allInfected.insert(newId);
                activeInfected.insert(newId);
            }
        }

        result.totalInfected = static_cast<int>(allInfected.size());

        SimulationStepLog logEntry;
        logEntry.step = currentStep;
        logEntry.newlyInfectedCount = static_cast<int>(newInfectionsThisStep.size());
        logEntry.totalInfected = result.totalInfected;
        logEntry.activeInfectedCount = static_cast<int>(activeInfected.size());
        logEntry.recoveredCount = static_cast<int>(recovered.size());
        result.stepLogs.push_back(logEntry);

        // Plateau detection
        if (newInfectionsThisStep.empty()) {
            consecutiveZeroInfections++;
            if (consecutiveZeroInfections >= plateauThreshold) {
                break;
            }
        } else {
            consecutiveZeroInfections = 0;
        }

        currentStep++;
    }

    result.stepsToPlateau = (currentStep <= maxSteps) ? currentStep : maxSteps;
    return result;
}
