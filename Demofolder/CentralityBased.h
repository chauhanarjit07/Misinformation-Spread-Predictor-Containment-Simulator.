#ifndef RUMORGUARD_CENTRALITY_BASED_H
#define RUMORGUARD_CENTRALITY_BASED_H

#include "Strategy.h"

class CentralityBased : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "Betweenness centrality"; }
};

#endif
