#ifndef RUMORGUARD_CENTRALITY_BASED_H
#define RUMORGUARD_CENTRALITY_BASED_H

#include "Strategy.h"

/**
 * @class CentralityBased
 * @brief Targets "bridge" nodes with the highest betweenness centrality scores.
 *
 * Algorithmic Justification:
 * - Betweenness centrality quantifies the fraction of all shortest paths passing through a node.
 * - While Degree-Based strategy targets dense hubs, Centrality-Based targets strategic bridges
 *   connecting separate network clusters, fragmenting the network and halting cross-community spread.
 */
class CentralityBased : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "Betweenness centrality"; }
};

#endif // RUMORGUARD_CENTRALITY_BASED_H