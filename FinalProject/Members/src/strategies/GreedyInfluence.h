#ifndef RUMORGUARD_GREEDY_INFLUENCE_H
#define RUMORGUARD_GREEDY_INFLUENCE_H

#include "Strategy.h"
#include <vector>

/**
 * @class GreedyInfluence
 * @brief Greedy Influence Maximization heuristic based on marginal spread reduction.
 *
 * Algorithmic Justification:
 * - Optimal influence containment on graphs is NP-hard (Kempe, Kleinberg, Tardos, 2003).
 * - A greedy approximation with submodular properties achieves a (1 - 1/e) ~ 63% approximation bound.
 * - We maintain a std::priority_queue (Max-Heap) for candidate selection, evaluating the
 *   marginal susceptibility reduction of 1-hop and 2-hop neighbor neighborhoods.
 */
class GreedyInfluence : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "Greedy influence"; }

private:
    double estimateMarginalGain(const Graph& graph, int candidateId, const std::vector<int>& currentSelected) const;
};

#endif // RUMORGUARD_GREEDY_INFLUENCE_H
