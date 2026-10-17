#ifndef RUMORGUARD_GRAPH_H
#define RUMORGUARD_GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>

/**
 * @struct Node
 * @brief Represents an individual in the information spread network.
 *
 * Data Structure Choice:
 * - Flat struct with primitive fields for cache locality.
 * - trustScore: Double in [0, 1] representing skepticism toward unverified rumors.
 * - activityLevel: Double in [0, 1] representing frequency of interaction and spreading tendency.
 * - Betweenness and Clustering metrics cached directly on node for fast strategy lookup.
 */
struct Node {
    int id;
    double trustScore;           // In [0, 1]: Higher trust score = higher skepticism, lower infection probability
    double activityLevel;        // In [0, 1]: Higher activity = higher rate of sharing / contacting neighbors
    bool isInfected;             // True if currently carrying and transmitting the rumor
    bool isVaccinated;           // True if immunized / fact-checked (cannot be infected)
    int infectedAtStep;          // Timestep when infection occurred (-1 if uninfected)
    int degree;                  // Number of connected neighbors
    double clusteringCoefficient;// Ratio of existing neighbor connections to total possible
    double betweennessCentrality;// Shortest path traversal score via Brandes' algorithm

    Node()
        : id(-1), trustScore(0.5), activityLevel(0.5),
          isInfected(false), isVaccinated(false),
          infectedAtStep(-1), degree(0),
          clusteringCoefficient(0.0), betweennessCentrality(0.0) {}

    explicit Node(int nodeId)
        : id(nodeId), trustScore(0.5), activityLevel(0.5),
          isInfected(false), isVaccinated(false),
          infectedAtStep(-1), degree(0),
          clusteringCoefficient(0.0), betweennessCentrality(0.0) {}
};

/**
 * @class Graph
 * @brief Adjacency list representation of an undirected social network.
 *
 * Algorithmic & Data Structure Justification:
 * - Adjacency list via std::unordered_map<int, std::vector<int>> provides O(1) average
 *   node lookup and O(deg(v)) neighbor iteration.
 * - Chosen over Adjacency Matrix (O(V^2) space) because social networks are sparse (E << V^2).
 *   For V=500 and average degree ~6, adjacency list uses ~6KB vs 250KB matrix and enables
 *   fast BFS traversals during simulation and Brandes' algorithm.
 */
class Graph {
public:
    std::unordered_map<int, Node> nodes;
    std::unordered_map<int, std::vector<int>> adj;

    Graph() = default;

    void addNode(int id);
    void addEdge(int u, int v);
    std::vector<int> getNeighbors(int id) const;
    int size() const;
    Node& getNode(int id);
    const Node& getNode(int id) const;
    bool hasNode(int id) const;
    void resetStates(); // Resets infection and vaccination states for fresh simulation runs
};

#endif // RUMORGUARD_GRAPH_H
