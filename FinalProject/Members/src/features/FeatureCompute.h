#ifndef RUMORGUARD_FEATURE_COMPUTE_H
#define RUMORGUARD_FEATURE_COMPUTE_H

#include "../graph/Graph.h"

/**
 * @brief Computes node degree for all nodes in the graph.
 * Time Complexity: O(V)
 */
void computeDegree(Graph& graph);

/**
 * @brief Computes local clustering coefficient for every node.
 *
 * Formula: C(v) = (2 * E_neighbors(v)) / (deg(v) * (deg(v) - 1))
 * Meaning: Measures triadic closure / local community density around a node.
 * Time Complexity: O(V * (deg_avg)^2) = O(V * (E/V)^2) = O(E^2 / V)
 */
void computeClusteringCoefficient(Graph& graph);

/**
 * @brief Computes Betweenness Centrality using Brandes' Algorithm (2001).
 *
 * Algorithmic Justification:
 * - Brandes' algorithm computes betweenness centrality in O(V * (V + E)) time for unweighted graphs,
 *   drastically outperforming naive all-pairs shortest paths approaches (Floyd-Warshall: O(V^3)).
 * - Misinformation brokers are often bridging nodes with high betweenness centrality rather than
 *   just high degree. Targeting them cuts communication pathways between disparate clusters.
 */
void computeBetweennessCentrality(Graph& graph);

#endif // RUMORGUARD_FEATURE_COMPUTE_H
