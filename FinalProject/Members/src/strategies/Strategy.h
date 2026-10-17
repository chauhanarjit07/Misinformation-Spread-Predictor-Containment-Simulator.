#ifndef RUMORGUARD_STRATEGY_H
#define RUMORGUARD_STRATEGY_H

#include "../graph/Graph.h"
#include <vector>
#include <string>

/**
 * @class Strategy
 * @brief Abstract base class defining the containment strategy interface.
 *
 * Design Pattern: Strategy Pattern (OOP)
 * - Enables runtime polymorphism and clean separation of algorithmic strategies.
 * - Allows benchmarking multiple heuristic approaches against a standard evaluation harness.
 */
class Strategy {
public:
    virtual ~Strategy() = default;

    /**
     * Selects nodes to be targeted/immunized within a given budget percentage.
     * @param graph The social network graph
     * @param budgetPercent Integer percentage of total nodes allowed for intervention (e.g., 5, 10, 20)
     * @return List of selected node IDs
     */
    virtual std::vector<int> selectTargets(Graph& graph, int budgetPercent) = 0;

    /**
     * @return Human-readable strategy name
     */
    virtual std::string name() const = 0;
};

#endif // RUMORGUARD_STRATEGY_H
