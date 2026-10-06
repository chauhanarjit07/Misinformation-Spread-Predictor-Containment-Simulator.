#pragma once
#include <vector>
#include <algorithm>

// Stores a node and its score
struct NodeScore {
    int node;
    double score;
};

// Base class for containment strategies
class Strategy {
public:

    // Gives the name of the strategy
    virtual const char* name() const = 0;

    // Selects nodes according to the given budget
    virtual std::vector<int> select(
        const std::vector<NodeScore>& nodes,
        int budget) = 0;
};

// Centrality-based strategy
// Selects nodes with the highest scores
class CentralityStrategy : public Strategy {
public:

    // Returns the strategy name
    const char* name() const override {
        return "Centrality";
    }

    // Select nodes according to the budget
    std::vector<int> select(
        const std::vector<NodeScore>& nodes,
        int budget) override {

        // Make a copy so the original list is not changed
        std::vector<NodeScore> ranked = nodes;

        // Sort nodes from highest score to lowest score
        sort(ranked.begin(), ranked.end(),
            [](NodeScore a, NodeScore b) {
                return a.score > b.score;
            });

        // Store the selected nodes
        std::vector<int> selected;

        // The budget limits how many nodes can be selected
        // for containment
        for (int i = 0; i < budget && i < ranked.size(); i++)
            selected.push_back(ranked[i].node);

        return selected;
    }
};