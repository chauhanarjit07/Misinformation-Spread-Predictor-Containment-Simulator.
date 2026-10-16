#include "FeatureCompute.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

void computeDegree(Graph& graph) {
    for (auto& pair : graph.nodes) {
        pair.second.degree = static_cast<int>(graph.getNeighbors(pair.first).size());
    }
}

void computeClusteringCoefficient(Graph& graph) {
    for (auto& pair : graph.nodes) {
        int u = pair.first;
        const std::vector<int>& neighbors = graph.getNeighbors(u);
        int k = static_cast<int>(neighbors.size());

        if (k < 2) {
            pair.second.clusteringCoefficient = 0.0;
            continue;
        }

        // Put neighbors into a hash set for O(1) adjacency checks
        std::unordered_set<int> neighborSet(neighbors.begin(), neighbors.end());
        int existingEdges = 0;

        for (size_t i = 0; i < neighbors.size(); ++i) {
            int v = neighbors[i];
            const std::vector<int>& vNeighbors = graph.getNeighbors(v);
            for (int w : vNeighbors) {
                // To count each undirected edge once, enforce ordering
                if (w > v && neighborSet.find(w) != neighborSet.end()) {
                    existingEdges++;
                }
            }
        }

        int possibleEdges = (k * (k - 1)) / 2;
        pair.second.clusteringCoefficient = (possibleEdges > 0)
            ? static_cast<double>(existingEdges) / possibleEdges
            : 0.0;
    }
}

/**
 * Brandes' Algorithm (2001) Implementation:
 *
 * Algorithm Structure:
 * For each source node s in V:
 *   1. Forward BFS Pass (Shortest paths & predecessor DAG generation):
 *      - Discovers shortest path distances d(s, v) from s to all v.
 *      - Computes number of shortest paths sigma(s, v).
 *      - Records list of predecessors Pred(v) on shortest paths from s.
 *      - Pushes nodes onto stack S in non-decreasing order of distance.
 *
 *   2. Backward Accumulation Pass:
 *      - Pops vertices w from stack S in reverse order of discovery (leaves to source).
 *      - Accumulates pair-dependency delta(s, v) = sum_{w: v in Pred(w)} (sigma(s, v) / sigma(s, w)) * (1 + delta(s, w)).
 *      - Adds delta(s, v) to total betweenness centrality of v.
 *
 * Total Time Complexity: V * O(V + E) = O(V^2 + V*E). For sparse graphs, O(V^2).
 */
void computeBetweennessCentrality(Graph& graph) {
    // Reset centrality values
    for (auto& pair : graph.nodes) {
        pair.second.betweennessCentrality = 0.0;
    }

    int n = graph.size();
    if (n <= 2) return;

    // Collect all node IDs
    std::vector<int> allNodeIds;
    allNodeIds.reserve(n);
    for (const auto& pair : graph.nodes) {
        allNodeIds.push_back(pair.first);
    }

    for (int s : allNodeIds) {
        // Data structures for this single-source shortest path iteration
        std::stack<int> S; // Stack to record order of visits for backward accumulation
        std::unordered_map<int, std::vector<int>> pred; // Predecessors on shortest paths from s
        std::unordered_map<int, double> sigma;          // Number of shortest paths from s to v
        std::unordered_map<int, int> dist;              // Shortest path distance from s to v

        for (int v : allNodeIds) {
            pred[v] = std::vector<int>();
            sigma[v] = 0.0;
            dist[v] = -1; // -1 represents infinity / unvisited
        }

        sigma[s] = 1.0;
        dist[s] = 0;

        std::queue<int> Q;
        Q.push(s);

        // ==========================================
        // PHASE 1: Forward BFS Pass
        // ==========================================
        while (!Q.empty()) {
            int v = Q.front();
            Q.pop();
            S.push(v);

            for (int w : graph.getNeighbors(v)) {
                // Path discovery: w is found for the first time
                if (dist[w] < 0) {
                    dist[w] = dist[v] + 1;
                    Q.push(w);
                }

                // Path counting: shortest path to w via v
                if (dist[w] == dist[v] + 1) {
                    sigma[w] += sigma[v];
                    pred[w].push_back(v);
                }
            }
        }

        // ==========================================
        // PHASE 2: Backward Accumulation Pass
        // ==========================================
        std::unordered_map<int, double> delta;
        for (int v : allNodeIds) {
            delta[v] = 0.0;
        }

        // S returns vertices in order of non-increasing distance from s
        while (!S.empty()) {
            int w = S.top();
            S.pop();

            for (int v : pred[w]) {
                if (sigma[w] > 0.0) {
                    delta[v] += (sigma[v] / sigma[w]) * (1.0 + delta[w]);
                }
            }

            if (w != s) {
                graph.getNode(w).betweennessCentrality += delta[w];
            }
        }
    }

    // For undirected graphs, divide by 2 because each path s -> t and t -> s is counted twice
    for (auto& pair : graph.nodes) {
        pair.second.betweennessCentrality /= 2.0;
    }
}
