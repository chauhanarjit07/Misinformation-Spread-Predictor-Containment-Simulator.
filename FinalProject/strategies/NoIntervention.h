#ifndef RUMORGUARD_NO_INTERVENTION_H
#define RUMORGUARD_NO_INTERVENTION_H

#include "Strategy.h"

/**
 * @class NoIntervention
 * @brief Baseline control strategy that performs zero containment.
 * Serves as the benchmark to measure percentage reduction of rumors.
 */
class NoIntervention : public Strategy {
public:
    std::vector<int> selectTargets(Graph& graph, int budgetPercent) override;
    std::string name() const override { return "No intervention"; }
};

#endif // RUMORGUARD_NO_INTERVENTION_H
