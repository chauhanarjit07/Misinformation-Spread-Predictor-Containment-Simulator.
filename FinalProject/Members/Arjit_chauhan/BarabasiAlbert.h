#ifndef RUMORGUARD_BARABASI_ALBERT_H
#define RUMORGUARD_BARABASI_ALBERT_H

#include "Graph.h"

/**
 * @brief Generates a scale-free network using the Barabási–Albert model.
 *
 * Algorithmic Justification:
 * - Real-world social networks exhibit power-law degree distributions (P(k) ~ k^(-gamma)),
 *   meaning a small number of hub nodes have disproportionately large influence on rumor spread.
 * - Preferential attachment simulates the "rich-get-richer" dynamic of social media followers.
 */

void createSeedNetwork(Graph& graph, int m0);
Graph generateBarabasiAlbert(int totalNodes, int m, unsigned int seed = 42);

#endif // RUMORGUARD_BARABASI_ALBERT_H
