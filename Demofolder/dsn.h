#pragma once
#include <utility>
#include <vector>

// FIFO queue over node ids. Each id is pushed at most once between reset() calls, so capacity n never overflows.
// buf[0..tail) doubles as the BFS visit order.
class IntQueue {
public:
    std::vector<int> buf;
    int head, tail;
    explicit IntQueue(int cap = 0) : buf(cap), head(0), tail(0) {}
    void reset() { head = tail = 0; }
    void push(int x) { buf[tail++] = x; }
    int pop() { return buf[head++]; }
    bool empty() const { return head == tail; }
};

// Disjoint Set Union: path halving + union by size.
class DSU {
    std::vector<int> parent, sz;
public:
    explicit DSU(int n) : parent(n), sz(n, 1) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
        return true;
    }
    int size(int x) { return sz[find(x)]; }
};