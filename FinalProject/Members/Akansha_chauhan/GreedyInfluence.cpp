#include "GreedyInfluence.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

/**
 * Marginal Gain Estimator:
 * Approximates how much contagion risk is mitigated by vaccinating `candidateId`.
 * Evaluates the node itself plus the weighted susceptibility of its unprotected 1-hop neighbors:
 * Gain = (Susceptibility_cand * Activity_cand) + sum_{nbr} (Susceptibility_nbr * Activity_cand)
 */
double GreedyInfluence::estimateMarginalGain(const Graph& graph, int candidateId, const std::vector<int>& currentSelected) const {
    if (!graph.hasNode(candidateId)) return 0.0;

    std::unordered_set<int> selectedSet(currentSelected.begin(), currentSelected.end());
    if (selectedSet.find(candidateId) != selectedSet.end()) return 0.0;

    const Node& candNode = graph.getNode(candidateId);
    double candRisk = (1.0 - candNode.trustScore * 0.7) * (0.3 + 0.7 * candNode.activityLevel);
    double totalGain = candRisk;

    const std::vector<int>& neighbors = graph.getNeighbors(candidateId);
    for (int nbrId : neighbors) {
        if (selectedSet.find(nbrId) == selectedSet.end()) {
            const Node& nbrNode = graph.getNode(nbrId);
            double nbrSusceptibility = (1.0 - nbrNode.trustScore * 0.7);
            // Marginal risk blocked along edge (candidate -> neighbor)
            totalGain += nbrSusceptibility * candNode.activityLevel * 0.6;
        }
    }

    return totalGain;
}

std::vector<int> GreedyInfluence::selectTargets(Graph& graph, int budgetPercent) {
    int totalNodes = graph.size();
    if (totalNodes == 0 || budgetPercent <= 0) return {};

    int targetCount = std::max(1, (totalNodes * budgetPercent) / 100);
    targetCount = std::min(targetCount, totalNodes);

    std::vector<int> selected;
    selected.reserve(targetCount);

    std::unordered_set<int> candidatePool;
    for (const auto& pair : graph.nodes) {
        candidatePool.insert(pair.first);
    }

    // Element in max-heap: pair<gain, nodeId>
    using HeapElement = std::pair<double, int>;

    for (int step = 0; step < targetCount && !candidatePool.empty(); ++step) {
        // Max-heap prioritizing highest marginal reduction
        std::priority_queue<HeapElement> maxHeap;

        for (int candId : candidatePool) {
            double gain = estimateMarginalGain(graph, candId, selected);
            maxHeap.push({gain, candId});
        }

        if (maxHeap.empty()) break;

        int bestNode = maxHeap.top().second;
        selected.push_back(bestNode);
        candidatePool.erase(bestNode);
    }

    return selected;
}
