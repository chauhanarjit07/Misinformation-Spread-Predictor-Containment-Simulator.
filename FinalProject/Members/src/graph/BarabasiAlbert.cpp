#include "BarabasiAlbert.h"
#include <random>
#include <vector>
#include <unordered_set>
#include <algorithm>

/**
 * Creates an initial fully-connected seed network with m0 nodes.
 * A clique of size m0 ensures every initial node has non-zero degree (m0 - 1),
 * preventing division-by-zero or zero-probability stalls in preferential attachment.
 */
void createSeedNetwork(Graph& graph, int m0) {
    for (int i = 0; i < m0; ++i) {
        graph.addNode(i);
    }
    for (int i = 0; i < m0; ++i) {
        for (int j = i + 1; j < m0; ++j) {
            graph.addEdge(i, j);
        }
    }
}

/**
 * Generates a Barabási–Albert scale-free network.
 *
 * Degree-Proportional Sampling Method Chosen:
 * We use a "repeated-nodes degree bucket" approach (equivalent to roulette-wheel sampling
 * via cumulative frequency distribution).
 *
 * Explanation:
 * - A vector `nodePool` contains each node id repeated exactly `deg(v)` times.
 * - Total elements in `nodePool` = sum of all degrees = 2 * E.
 * - Sampling a random index uniformly from `nodePool` selects node `v` with probability:
 *     P(v) = deg(v) / (2 * E)
 * - This achieves O(1) sampling time per candidate check and avoids floating-point precision
 *   issues associated with cumulative probability sums, while requiring only O(E) extra space.
 */
Graph generateBarabasiAlbert(int totalNodes, int m, unsigned int seed) {
    Graph graph;
    if (totalNodes <= 0) return graph;

    int m0 = std::max(2, m);
    if (totalNodes <= m0) {
        createSeedNetwork(graph, totalNodes);
        return graph;
    }

    createSeedNetwork(graph, m0);

    // Maintain the degree-proportional sampling pool
    std::vector<int> nodePool;
    for (int i = 0; i < m0; ++i) {
        int deg = static_cast<int>(graph.getNeighbors(i).size());
        for (int d = 0; d < deg; ++d) {
            nodePool.push_back(i);
        }
    }

    std::mt19937 rng(seed);

    // Iteratively attach new nodes (m0 to totalNodes - 1)
    for (int i = m0; i < totalNodes; ++i) {
        graph.addNode(i);

        std::unordered_set<int> chosenNeighbors;
        int maxAttempts = m * 50;
        int attempts = 0;

        while (static_cast<int>(chosenNeighbors.size()) < m && attempts < maxAttempts && !nodePool.empty()) {
            std::uniform_int_distribution<size_t> dist(0, nodePool.size() - 1);
            int candidate = nodePool[dist(rng)];

            if (candidate != i && chosenNeighbors.find(candidate) == chosenNeighbors.end()) {
                chosenNeighbors.insert(candidate);
            }
            attempts++;
        }

        // Fallback in rare case sampling takes too many attempts: pick highest degree unselected nodes
        if (static_cast<int>(chosenNeighbors.size()) < m) {
            for (int j = 0; j < i && static_cast<int>(chosenNeighbors.size()) < m; ++j) {
                if (chosenNeighbors.find(j) == chosenNeighbors.end()) {
                    chosenNeighbors.insert(j);
                }
            }
        }

        // Add edges and update degree sampling pool
        for (int neighbor : chosenNeighbors) {
            graph.addEdge(i, neighbor);
            nodePool.push_back(i);
            nodePool.push_back(neighbor);
        }
    }

    return graph;
}
