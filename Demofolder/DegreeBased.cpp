#include "DegreeBased.h"
#include <algorithm>
#include <vector>

std::vector<int> DegreeBased::selectTargets(Graph& graph, int budgetPercent) {
    int totalNodes = graph.size();
    if (totalNodes == 0 || budgetPercent <= 0) return {};

    int targetCount = std::max(1, (totalNodes * budgetPercent) / 100);
    targetCount = std::min(targetCount, totalNodes);

    // Vector of pairs: (degree, nodeId)
    std::vector<std::pair<int, int>> nodeDegreeList;
    nodeDegreeList.reserve(totalNodes);

    for (const auto& pair : graph.nodes) {
        nodeDegreeList.push_back({pair.second.degree, pair.first});
    }

    // Sort descending by degree. O(V log V) time complexity
    std::sort(nodeDegreeList.begin(), nodeDegreeList.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  if (a.first != b.first) {
                      return a.first > b.first;
                  }
                  return a.second < b.second; // Deterministic tie-breaker
              });

    std::vector<int> selected;
    selected.reserve(targetCount);
    for (int i = 0; i < targetCount && i < static_cast<int>(nodeDegreeList.size()); ++i) {
        selected.push_back(nodeDegreeList[i].second);
    }

    return selected;
}