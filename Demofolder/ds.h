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

// Binary max-heap. Order: larger key first, ties broken by smaller id (deterministic rankings).
// stamp is used by the lazy-greedy strategy to record the round in which key was computed.
struct HeapItem {
    double key;
    int id;
    int stamp;
};

class MaxHeap {
    std::vector<HeapItem> a;

    static bool better(const HeapItem& x, const HeapItem& y) {
        return x.key > y.key || (x.key == y.key && x.id < y.id);
    }
    void siftUp(int i) {
        HeapItem it = a[i];
        while (i > 0) {
            int p = (i - 1) / 2;
            if (!better(it, a[p])) break;
            a[i] = a[p];
            i = p;
        }
        a[i] = it;
    }
    void siftDown(int i) {
        int n = (int)a.size();
        HeapItem it = a[i];
        while (true) {
            int l = 2 * i + 1;
            if (l >= n) break;
            int r = l + 1;
            int c = (r < n && better(a[r], a[l])) ? r : l;
            if (!better(a[c], it)) break;
            a[i] = a[c];
            i = c;
        }
        a[i] = it;
    }

public:
    bool empty() const { return a.empty(); }
    int size() const { return (int)a.size(); }
    void push(const HeapItem& it) {
        a.push_back(it);
        siftUp((int)a.size() - 1);
    }
    HeapItem pop() {
        HeapItem top = a[0];
        a[0] = a.back();
        a.pop_back();
        if (!a.empty()) siftDown(0);
        return top;
    }
};
