#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include "../graph/Graph.h"
#include "../graph/BarabasiAlbert.h"
#include "../features/FeatureCompute.h"
#include "../simulation/SpreadSimulator.h"
#include "../strategies/Strategy.h"
#include "../strategies/NoIntervention.h"
#include "../strategies/DegreeBased.h"
#include "../strategies/CentralityBased.h"
#include "../strategies/GreedyInfluence.h"

// Windows console color support
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

// Helper struct for aggregated summary
struct StrategyResult {
    std::string strategyName;
    int budgetPercent;
    double avgTotalInfected;
    double avgStepsToPlateau;
    double reductionPercent;
};

int main(int argc, char* argv[]) {
    // Default CLI Parameters
    int totalNodes = 500;
    int mEdges = 3;
    int trialsPerConfig = 20;
    std::string outDir = "results/";
    std::string targetStrategy = "";
    int targetBudget = -1;

    // CLI argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--nodes" && i + 1 < argc) {
            totalNodes = std::stoi(argv[++i]);
        } else if (arg == "--m" && i + 1 < argc) {
            mEdges = std::stoi(argv[++i]);
        } else if (arg == "--trials" && i + 1 < argc) {
            trialsPerConfig = std::stoi(argv[++i]);
        } else if (arg == "--outdir" && i + 1 < argc) {
            outDir = argv[++i];
            if (!outDir.empty() && outDir.back() != '/' && outDir.back() != '\\') {
                outDir += "/";
            }
        } else if (arg == "--strategy" && i + 1 < argc) {
            targetStrategy = argv[++i];
        } else if (arg == "--budget" && i + 1 < argc) {
            targetBudget = std::stoi(argv[++i]);
        }
    }

    std::cout << "===============================================================\n";
    std::cout << "  RumorGuard: Misinformation Spread & Containment Simulator   \n";
    std::cout << "===============================================================\n";
    std::cout << "Configuration:\n";
    std::cout << "  - Network Size (Nodes) : " << totalNodes << "\n";
    std::cout << "  - BA Edges per New Node: " << mEdges << "\n";
    std::cout << "  - Trials per Strategy  : " << trialsPerConfig << "\n";
    std::cout << "  - Output Directory     : " << outDir << "\n";
    std::cout << "---------------------------------------------------------------\n";

    bool runSpecific = !targetStrategy.empty() && targetBudget >= 0;

    // 1. Generate Graph using Barabási–Albert Preferential Attachment
    std::cout << "[1/4] Generating Barabasi-Albert scale-free network...\n";
    Graph graph = generateBarabasiAlbert(totalNodes, mEdges, 42);
    std::cout << "      Network generated with " << graph.size() << " nodes.\n";

    // If specific strategy and budget requested, echo that in the console output
    if (runSpecific) {
        std::cout << "  - Target Strategy: " << targetStrategy << "\n";
        std::cout << "  - Target Budget: " << targetBudget << "%\n";
    }

    // 2. Feature Computation
    std::cout << "[2/4] Computing topological features...\n";
    std::cout << "      - Node Degrees... ";
    computeDegree(graph);
    std::cout << "Done.\n";

    std::cout << "      - Local Clustering Coefficients... ";
    computeClusteringCoefficient(graph);
    std::cout << "Done.\n";

    std::cout << "      - Betweenness Centrality (Brandes' Algorithm O(V*(V+E)))... ";
    computeBetweennessCentrality(graph);
    std::cout << "Done.\n";

    // 3. Initialize Strategies
    std::vector<std::unique_ptr<Strategy>> strategies;
    strategies.push_back(std::make_unique<NoIntervention>());
    strategies.push_back(std::make_unique<DegreeBased>());
    strategies.push_back(std::make_unique<CentralityBased>());
    strategies.push_back(std::make_unique<GreedyInfluence>());

    std::vector<int> budgets = {5, 10, 20};

    // Save strategy data for frontend
    std::ofstream dataFile(outDir + "strategy_data.json");
    if (!dataFile.is_open()) {
        std::cerr << "Error: Unable to open " << outDir << "strategy_data.json for writing.\n";
        return 1;
    }

    // Open comparison_results.csv only if running all combinations
    std::ofstream compFile;
    if (!runSpecific) {
        compFile.open(outDir + "comparison_results.csv");
        if (!compFile.is_open()) {
            std::cerr << "Error: Unable to open " << outDir << "comparison_results.csv for writing.\n";
            return 1;
        }
        compFile << "Strategy,BudgetPercent,AvgTotalInfected,AvgStepsToPlateau\n";
    }

    // Open simulation_log.csv only if running all combinations
    std::ofstream logFile;
    if (!runSpecific) {
        logFile.open(outDir + "simulation_log.csv");
        if (!logFile.is_open()) {
            std::cerr << "Error: Unable to open " << outDir << "simulation_log.csv for writing.\n";
            return 1;
        }
        logFile << "Strategy,BudgetPercent,Run,TotalInfected,StepsToPlateau\n";
    }

    // 4. Run Simulations & Evaluations
    std::cout << "[3/4] Running simulation trials across strategies and budgets...\n";
    SpreadSimulator simulator(5, 100, 3, 0.45);
    std::vector<StrategyResult> summaryResults;
    double baselineInfected = 0.0;

    // First pass: collect data for JSON file
    dataFile << "{\"strategies\": [" << std::endl;
    bool firstJSONRow = true;

    for (const auto& strat : strategies) {
        for (int budget : budgets) {
            if (runSpecific && (strat->name() != targetStrategy || budget != targetBudget)) {
                continue; // Skip this combination
            }
            // For No Intervention, budget does not change target set (empty), but we evaluate for completeness
            std::vector<int> targets = strat->selectTargets(graph, budget);

            double sumInfected = 0.0;
            double sumSteps = 0.0;

            for (int trial = 0; trial < trialsPerConfig; ++trial) {
                // Independent trial seed for statistical variance
                unsigned int trialSeed = 1000 + trial * 37 + budget * 101;
                SimulationResult res = simulator.runSimulation(graph, targets, trialSeed);

                sumInfected += res.totalInfected;
                sumSteps += res.stepsToPlateau;

                if (!runSpecific && logFile.is_open()) {
                    logFile << strat->name() << "," << budget << "," << trial << ","
                            << res.totalInfected << "," << res.stepsToPlateau << "\n";
                }
            }

            double avgInf = sumInfected / trialsPerConfig;
            double avgStp = sumSteps / trialsPerConfig;

            if (strat->name() == "No intervention" && baselineInfected == 0.0) {
                baselineInfected = avgInf;
            }

            double reduction = 0.0;
            if (baselineInfected > 0.0) {
                double raw = ((baselineInfected - avgInf) / baselineInfected) * 100.0;
                reduction = raw > 0.0 ? raw : 0.0;
            }

            // Write to strategy_data.json
            if (!firstJSONRow) {
                dataFile << "," << std::endl;
            }
            firstJSONRow = false;
            dataFile << "  {\"name\": \"" << strat->name() << "\", \"budgetPercent\": " << budget
                     << ", \"avgTotalInfected\": " << std::fixed << std::setprecision(1) << avgInf
                     << ", \"avgStepsToPlateau\": " << std::fixed << std::setprecision(1) << avgStp
                     << ", \"reductionPercent\": " << std::fixed << std::setprecision(0) << reduction << "}";

            // Write to comparison_results.csv only if running all combinations
            if (!runSpecific && compFile.is_open()) {
                compFile << strat->name() << "," << budget << ","
                         << std::fixed << std::setprecision(1) << avgInf << ","
                         << std::fixed << std::setprecision(1) << avgStp << "\n";
            }

            summaryResults.push_back({strat->name(), budget, avgInf, avgStp, reduction});
        }
    }

    dataFile << "\n]" << std::endl;
    dataFile.close();

    // 5. Console Summary Output
    std::cout << "[4/4] Writing output files and generating console report...\n\n";
    std::cout << "=========================================================================\n";
    std::cout << "                    STRATEGY EVALUATION SUMMARY TABLE                    \n";
    std::cout << "=========================================================================\n";
    std::cout << std::left << std::setw(24) << "Strategy"
              << std::setw(10) << "Budget"
              << std::setw(16) << "Avg Infected"
              << std::setw(16) << "Avg Plateau"
              << std::setw(12) << "Reduction" << "\n";
    std::cout << "-------------------------------------------------------------------------\n";

    for (const auto& row : summaryResults) {
        std::cout << std::left << std::setw(24) << row.strategyName
                  << std::setw(10) << (std::to_string(row.budgetPercent) + "%")
                  << std::setw(16) << std::fixed << std::setprecision(1) << row.avgTotalInfected
                  << std::setw(16) << std::fixed << std::setprecision(1) << row.avgStepsToPlateau
                  << std::setw(12) << (std::to_string(static_cast<int>(std::round(row.reductionPercent))) + "%")
                  << "\n";
    }
    std::cout << "=========================================================================\n";
    std::cout << "Files successfully created:\n";
    std::cout << "  - " << outDir << "strategy_data.json\n";
    if (!runSpecific) {
        std::cout << "  - " << outDir << "comparison_results.csv\n";
        std::cout << "  - " << outDir << "simulation_log.csv\n";
    }
    std::cout << "To visualize results, open ui/index.html in your browser.\n\n";

    compFile.close();
    logFile.close();

    return 0;
}
