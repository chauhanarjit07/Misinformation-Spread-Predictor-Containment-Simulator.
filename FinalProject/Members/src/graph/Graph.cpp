#include "Graph.h"
#include <cmath>
#include <algorithm>

void Graph::addNode(int id) {
    if (nodes.find(id) == nodes.end()) {
        Node node(id);
        // Deterministic pseudo-random generation based on id for reproducibility in course testing
        double tVal = std::sin(id * 12.9898 + 78.233) * 43758.5453;
        node.trustScore = std::abs(tVal - std::floor(tVal)); // [0, 1)

        double aVal = std::sin(id * 39.3467 + 11.135) * 23421.6312;
        node.activityLevel = std::abs(aVal - std::floor(aVal)); // [0, 1)

        nodes[id] = node;
        adj[id] = std::vector<int>();
    }
}

void Graph::addEdge(int u, int v) {
    if (u == v) return; // Disallow self-loops
    addNode(u);
    addNode(v);

    // Prevent duplicate multi-edges in adjacency lists
    auto& uNeighbors = adj[u];
    if (std::find(uNeighbors.begin(), uNeighbors.end(), v) == uNeighbors.end()) {
        uNeighbors.push_back(v);
        nodes[u].degree++;
    }

    auto& vNeighbors = adj[v];
    if (std::find(vNeighbors.begin(), vNeighbors.end(), u) == vNeighbors.end()) {
        vNeighbors.push_back(u);
        nodes[v].degree++;
    }
}

std::vector<int> Graph::getNeighbors(int id) const {
    auto it = adj.find(id);
    if (it != adj.end()) {
        return it->second;
    }
    return {};
}

int Graph::size() const {
    return static_cast<int>(nodes.size());
}

Node& Graph::getNode(int id) {
    return nodes[id];
}

const Node& Graph::getNode(int id) const {
    auto it = nodes.find(id);
    return it->second;
}

bool Graph::hasNode(int id) const {
    return nodes.find(id) != nodes.end();
}

void Graph::resetStates() {
    for (auto& pair : nodes) {
        pair.second.isInfected = false;
        pair.second.isVaccinated = false;
        pair.second.infectedAtStep = -1;
    }
}
