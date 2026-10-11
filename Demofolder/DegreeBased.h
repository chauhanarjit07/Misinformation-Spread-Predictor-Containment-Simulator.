#ifndef RUMORGUARD_DEGREE_BASED_H
#define RUMORGUARD_DEGREE_BASED_H

#include "Strategy.h"

class DegreeBased : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "Degree-based"; }
};

#endif