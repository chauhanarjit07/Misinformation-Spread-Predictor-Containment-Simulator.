// RumorGuard: graph-based misinformation spread & containment simulator.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <string>
#include <vector>

#include "graph.h"
#include "rng.h"
#include "simulator.h"
#include "strategies.h"

struct Config {
    int n = 1000;
    int m = 3;
    double beta = 0.05;
    double gamma = 0.2;
    int seeds = 3;
    int scenarios = 5;
    int runs = 200;
    int samples = 40;
    uint64_t rngSeed = 42;
    std::vector<int> budgets = {5, 10, 20, 40, 80};
    std::string outDir = "output";
};

struct Acc {
    double sum = 0, sumSq = 0, plateau = 0, peak = 0;
    long cnt = 0;
    void add(const SimResult& r) {
        sum += r.total;
        sumSq += (double)r.total * (double)r.total;
        plateau += r.plateau;
        peak += r.peak;
        cnt++;
    }
    double mean() const { return sum / (double)cnt; }
    double stddev() const {
        double m = mean();
        double v = sumSq / (double)cnt - m * m;
        return v > 0 ? std::sqrt(v) : 0.0;
    }
    double meanPlateau() const { return plateau / (double)cnt; }
    double meanPeak() const { return peak / (double)cnt; }
};

static void die(const char* msg, const char* arg) {
    std::fprintf(stderr, "error: %s%s%s\n", msg, arg ? ": " : "", arg ? arg : "");
    std::exit(1);
}

static long parseLong(const char* s, const char* flag) {
    char* end = nullptr;
    long v = std::strtol(s, &end, 10);
    if (*s == '\0' || *end != '\0') die("invalid integer for", flag);
    return v;
}

static double parseDouble(const char* s, const char* flag) {
    char* end = nullptr;
    double v = std::strtod(s, &end);
    if (*s == '\0' || *end != '\0') die("invalid number for", flag);
    return v;
}

static void parseBudgets(const char* s, std::vector<int>& out) {
    out.clear();
    const char* p = s;
    while (*p) {
        char* end = nullptr;
        long v = std::strtol(p, &end, 10);
        if (end == p || v < 0) die("invalid --budgets list", s);
        if (v > 0) out.push_back((int)v);  // budget 0 is always reported as the "None" baseline
        p = end;
        if (*p == ',') p++;
        else if (*p != '\0') die("invalid --budgets list", s);
    }
    if (out.empty()) die("--budgets needs at least one positive value", nullptr);
}

static void usage() {
    std::printf(
        "Usage: rumorguard [options]\n"
        "  --nodes N        graph size (default 1000)\n"
        "  --m M            edges per new node in Barabasi-Albert (default 3)\n"
        "  --beta P         per-edge, per-step infection probability (default 0.05)\n"
        "  --gamma P        per-step recovery probability (default 0.2)\n"
        "  --seeds K        initially infected nodes per scenario (default 3)\n"
        "  --scenarios S    independent seed sets (default 5)\n"
        "  --runs R         Monte Carlo simulations per scenario/strategy/budget (default 200)\n"
        "  --samples L      live-edge samples used by the greedy strategy (default 40)\n"
        "  --budgets a,b,c  numbers of nodes that may be blocked (default 5,10,20,40,80)\n"
        "  --rng SEED       master random seed (default 42)\n"
        "  --out DIR        output directory (default output)\n");
}

int main(int argc, char** argv) {
    Config c;
    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];
        if (!std::strcmp(a, "--help") || !std::strcmp(a, "-h")) { usage(); return 0; }
        if (i + 1 >= argc) die("missing value for", a);
        const char* v = argv[++i];
        if (!std::strcmp(a, "--nodes")) c.n = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--m")) c.m = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--beta")) c.beta = parseDouble(v, a);
        else if (!std::strcmp(a, "--gamma")) c.gamma = parseDouble(v, a);
        else if (!std::strcmp(a, "--seeds")) c.seeds = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--scenarios")) c.scenarios = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--runs")) c.runs = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--samples")) c.samples = (int)parseLong(v, a);
        else if (!std::strcmp(a, "--rng")) c.rngSeed = (uint64_t)parseLong(v, a);
        else if (!std::strcmp(a, "--budgets")) parseBudgets(v, c.budgets);
        else if (!std::strcmp(a, "--out")) c.outDir = v;
        else die("unknown option", a);
    }

    int maxBudget = 0;
    for (int b : c.budgets) if (b > maxBudget) maxBudget = b;
    if (c.m < 1) die("--m must be >= 1", nullptr);
    if (c.n < c.m + 2) die("--nodes must be >= m + 2", nullptr);
    if (!(c.beta > 0.0 && c.beta <= 1.0)) die("--beta must be in (0,1]", nullptr);
    if (!(c.gamma > 0.0 && c.gamma <= 1.0)) die("--gamma must be in (0,1]", nullptr);
    if (c.seeds < 1) die("--seeds must be >= 1", nullptr);
    if (c.scenarios < 1 || c.runs < 1 || c.samples < 1) die("--scenarios, --runs, --samples must be >= 1", nullptr);
    if (c.seeds + maxBudget > c.n) die("--seeds + largest budget must not exceed --nodes", nullptr);

    std::filesystem::create_directories(c.outDir);
    std::clock_t t0 = std::clock();

    // ---- 1. Network generation and structural features -------------------------------------------------
    Rng graphRng(c.rngSeed);
    Graph g = Graph::barabasiAlbert(c.n, c.m, graphRng);
    g.computeAllFeatures();

    int largest = 0;
    int compsDSU = g.componentsDSU(&largest);
    int compsDFS = g.componentsDFS();
    if (compsDSU != compsDFS) die("internal check failed: DSU and DFS component counts differ", nullptr);

    int maxDeg = 0, maxDegNode = 0;
    double avgDeg = 0.0, avgClust = 0.0;
    for (int v = 0; v < g.n; v++) {
        if (g.nodes[v].degree > maxDeg) { maxDeg = g.nodes[v].degree; maxDegNode = v; }
        avgDeg += g.nodes[v].degree;
        avgClust += g.nodes[v].clustering;
    }
    avgDeg /= g.n;
    avgClust /= g.n;
    std::vector<int> dist;
    int ecc = g.bfsEccentricity(maxDegNode, dist);

    std::string nodesPath = c.outDir + "/nodes.csv";
    FILE* fn = std::fopen(nodesPath.c_str(), "w");
    if (!fn) die("cannot write", nodesPath.c_str());
    std::fprintf(fn, "node,degree,clustering,betweenness\n");
    for (int v = 0; v < g.n; v++)
        std::fprintf(fn, "%d,%d,%.6f,%.8f\n", v, g.nodes[v].degree, g.nodes[v].clustering, g.nodes[v].betweenness);
    std::fclose(fn);

    // per-edge transmission probability over an infectious period D ~ Geometric(gamma), D >= 1
    double T = 1.0 - c.gamma * (1.0 - c.beta) / (1.0 - (1.0 - c.gamma) * (1.0 - c.beta));

    std::printf("RumorGuard: graph-based misinformation spread & containment simulator\n");
    std::printf("Network : n=%d, edges=%d, m=%d, avg degree=%.2f, max degree=%d (node %d), avg clustering=%.4f\n",
                g.n, g.numEdges(), c.m, avgDeg, maxDeg, maxDegNode, avgClust);
    std::printf("          components=%d (largest=%d), eccentricity of top hub=%d hops\n", compsDSU, largest, ecc);
    std::printf("SIR     : beta=%.3f, gamma=%.3f (per-edge transmission prob T=%.3f)\n", c.beta, c.gamma, T);
    std::printf("Eval    : %d scenarios x %d runs, %d seeds each, greedy samples=%d, rng=%llu\n\n", c.scenarios, c.runs,
                c.seeds, c.samples, (unsigned long long)c.rngSeed);

    // ---- 2. Strategies ---------------------------------------------------------------------------------
    RandomStrategy rnd;
    DegreeStrategy deg;
    BetweennessStrategy btw;
    GreedyInfluenceStrategy greedy(c.samples, T);
    Strategy* strats[4] = {&rnd, &deg, &btw, &greedy};
    const int NS = 4;
    const int NB = (int)c.budgets.size();

    Acc accNone;
    std::vector<Acc> acc((size_t)NS * (size_t)NB);

    std::string tsPath = c.outDir + "/timeseries.csv";
    FILE* ft = std::fopen(tsPath.c_str(), "w");
    if (!ft) die("cannot write", tsPath.c_str());
    std::fprintf(ft, "strategy,budget,step,susceptible,infected,recovered,new_infected\n");
    std::vector<TraceRow> trace;

    SpreadSimulator sim(g, c.beta, c.gamma);

    // ---- 3. Scenarios: pick seeds, rank nodes per strategy, Monte Carlo every (strategy, budget) -----------
    for (int sc = 0; sc < c.scenarios; sc++) {
        Rng seedRng(c.rngSeed * 7919ULL + 1000ULL + (uint64_t)sc);
        std::vector<unsigned char> isSeed(g.n, 0);
        std::vector<int> seeds;
        while ((int)seeds.size() < c.seeds) {
            int v = seedRng.below(g.n);
            if (!isSeed[v]) { isSeed[v] = 1; seeds.push_back(v); }
        }

        std::vector<int> ranking[4];
        for (int s = 0; s < NS; s++) {
            Rng selRng(c.rngSeed * 104729ULL + 77ULL * (uint64_t)sc + (uint64_t)s);
            strats[s]->select(g, seeds, maxBudget, selRng, ranking[s]);
        }

        // Common random numbers: run r of scenario sc uses the same stream for every strategy/budget.
        const std::vector<int> none;
        for (int r = 0; r < c.runs; r++) {
            Rng simRng(c.rngSeed * 1000003ULL + (uint64_t)sc * 10007ULL + (uint64_t)r);
            bool tr = (sc == 0 && r == 0);
            SimResult res = sim.run(seeds, none, simRng, tr ? &trace : nullptr);
            accNone.add(res);
            if (tr)
                for (const TraceRow& row : trace)
                    std::fprintf(ft, "None,0,%d,%d,%d,%d,%d\n", row.step, row.susceptible, row.infected,
                                 row.recovered, row.newInfected);
        }
        for (int s = 0; s < NS; s++)
            for (int bi = 0; bi < NB; bi++) {
                int b = c.budgets[bi];
                std::vector<int> blocked(ranking[s].begin(), ranking[s].begin() + b);
                Acc& a = acc[(size_t)s * (size_t)NB + (size_t)bi];
                for (int r = 0; r < c.runs; r++) {
                    Rng simRng(c.rngSeed * 1000003ULL + (uint64_t)sc * 10007ULL + (uint64_t)r);
                    bool tr = (sc == 0 && r == 0);
                    SimResult res = sim.run(seeds, blocked, simRng, tr ? &trace : nullptr);
                    a.add(res);
                    if (tr)
                        for (const TraceRow& row : trace)
                            std::fprintf(ft, "%s,%d,%d,%d,%d,%d,%d\n", strats[s]->name(), b, row.step,
                                         row.susceptible, row.infected, row.recovered, row.newInfected);
                }
            }
        std::printf("scenario %d/%d done (seeds:", sc + 1, c.scenarios);
        for (int s : seeds) std::printf(" %d", s);
        std::printf(")\n");
        std::fflush(stdout);
    }
    std::fclose(ft);

    // ---- 4. Results ------------------------------------------------------------------------------------
    std::string resPath = c.outDir + "/results.csv";
    FILE* fr = std::fopen(resPath.c_str(), "w");
    if (!fr) die("cannot write", resPath.c_str());
    std::fprintf(fr, "strategy,budget,mean_total_infected,std_total_infected,mean_plateau_step,mean_peak_infected,"
                     "reduction_pct_vs_none\n");
    double base = accNone.mean();
    std::fprintf(fr, "None,0,%.3f,%.3f,%.3f,%.3f,0.00\n", base, accNone.stddev(), accNone.meanPlateau(),
                 accNone.meanPeak());
    for (int s = 0; s < NS; s++)
        for (int bi = 0; bi < NB; bi++) {
            const Acc& a = acc[(size_t)s * (size_t)NB + (size_t)bi];
            std::fprintf(fr, "%s,%d,%.3f,%.3f,%.3f,%.3f,%.2f\n", strats[s]->name(), c.budgets[bi], a.mean(),
                         a.stddev(), a.meanPlateau(), a.meanPeak(), 100.0 * (base - a.mean()) / base);
        }
    std::fclose(fr);

    std::printf("\nMean total infected (lower is better); baseline with no containment = %.1f of %d nodes\n", base, g.n);
    std::printf("%-8s", "Budget");
    for (int s = 0; s < NS; s++) std::printf("%14s", strats[s]->name());
    std::printf("%14s\n", "Best");
    for (int bi = 0; bi < NB; bi++) {
        std::printf("%-8d", c.budgets[bi]);
        int best = 0;
        for (int s = 0; s < NS; s++) {
            double m = acc[(size_t)s * (size_t)NB + (size_t)bi].mean();
            if (m < acc[(size_t)best * (size_t)NB + (size_t)bi].mean()) best = s;
            std::printf("%14.1f", m);
        }
        std::printf("%14s\n", strats[best]->name());
    }
    std::printf("\nMean time-to-plateau in steps (no containment = %.1f)\n", accNone.meanPlateau());
    std::printf("%-8s", "Budget");
    for (int s = 0; s < NS; s++) std::printf("%14s", strats[s]->name());
    std::printf("\n");
    for (int bi = 0; bi < NB; bi++) {
        std::printf("%-8d", c.budgets[bi]);
        for (int s = 0; s < NS; s++)
            std::printf("%14.1f", acc[(size_t)s * (size_t)NB + (size_t)bi].meanPlateau());
        std::printf("\n");
    }
    std::printf("\nWrote %s, %s, %s (%.1f s)\n", nodesPath.c_str(), resPath.c_str(), tsPath.c_str(),
                (double)(std::clock() - t0) / CLOCKS_PER_SEC);
    return 0;
}
