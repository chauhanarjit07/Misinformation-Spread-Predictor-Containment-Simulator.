#ifndef RUMORGUARD_DEGREE_BASED_H
#define RUMORGUARD_DEGREE_BASED_H

#include "Strategy.h"

/**
 * @class DegreeBased
 * @brief Targets high-degree "hub" nodes in the network.
 *
 * Algorithmic Justification:
 * - Degree centrality is an O(V log V) greedy heuristic targeting structural hubs.
 * - In scale-free networks with power-law degree distributions, hubs have high connectivity
 *   and act as primary amplifiers for rumor transmission.
 */
class DegreeBased : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "Degree-based"; }
};

#endif // RUMORGUARD_DEGREE_BASED_H
