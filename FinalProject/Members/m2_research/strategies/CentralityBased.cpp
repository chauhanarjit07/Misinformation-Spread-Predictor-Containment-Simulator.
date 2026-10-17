#include "CentralityBased.h"
#include <algorithm>
#include <vector>
#include <cmath>

std::vector<int> CentralityBased::selectTargets(Graph& graph, int budgetPercent) {
    int totalNodes = graph.size();
    if (totalNodes == 0 || budgetPercent <= 0) return {};

    int targetCount = std::max(1, (totalNodes * budgetPercent) / 100);
    targetCount = std::min(targetCount, totalNodes);

    // Vector of pairs: (betweennessCentrality, nodeId)
    std::vector<std::pair<double, int>> nodeCentralityList;
    nodeCentralityList.reserve(totalNodes);

    for (const auto& pair : graph.nodes) {
        nodeCentralityList.push_back({pair.second.betweennessCentrality, pair.first});
    }

    // Sort descending by betweenness centrality score
    std::sort(nodeCentralityList.begin(), nodeCentralityList.end(),
              [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                  if (std::abs(a.first - b.first) > 1e-9) {
                      return a.first > b.first;
                  }
                  return a.second < b.second; // Deterministic tie-breaker
              });

    std::vector<int> selected;
    selected.reserve(targetCount);
    for (int i = 0; i < targetCount && i < static_cast<int>(nodeCentralityList.size()); ++i) {
        selected.push_back(nodeCentralityList[i].second);
    }

    return selected;
}
