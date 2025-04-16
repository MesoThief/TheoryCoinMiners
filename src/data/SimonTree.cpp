#include "data/SimonTree.h"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace SimonTree;

Node::Node(int start, int end, int depth) : start(start), end(end), depth(depth) {}

Node::Node(int start, int end, int depth, std::shared_ptr<Node> parent)
    : start(start), end(end), depth(depth), parent(parent) {}

bool Node::isRoot() const { return parent == nullptr; }

bool Node::isLeaf() const { return children.empty(); }

// -------------------- Compute X[i] --------------------
vector<int> SimonTree::computeX(const string &w) {
    int n = w.size();
    vector<int> X(n + 2, n + 1);  // 1-based, X[n+1] = ∞
    unordered_map<char, int> lastSeen;

    for (int i = n; i >= 1; --i) {
        char c = w[i - 1];
        if (lastSeen.count(c)) {
            X[i] = lastSeen[c];
        }
        lastSeen[c] = i;
    }

    return X;
}

// -------------------- Find Node (Algorithm 2) --------------------
shared_ptr<Node> SimonTree::findNode(int i, const vector<int> &X, shared_ptr<Node> a) {
    while (!a->isRoot()) {
        int r = a->end;
        int rp = a->parent->end;
        if (X[i] >= r && X[i] < rp) {
            return a;
        } else {
            a->start = i + 1;  // Close block
            a = a->parent;
        }
    }
    return a;
}

// -------------------- Split Node (Algorithm 3) --------------------
pair<shared_ptr<Node>, shared_ptr<Node>> SimonTree::splitNode(int i, shared_ptr<Node> root, shared_ptr<Node> a) {
    if (a->isLeaf()) {
        // complete current leaf
        auto completed = make_shared<Node>(i + 1, i + 1, a->depth + 1, a);
        a->children.push_back(completed);
    }
    // add new open block [ ? : i ]
    auto newLeaf = make_shared<Node>(-1, i, a->depth + 1, a);
    a->children.insert(a->children.begin(), newLeaf);
    return {root, newLeaf};
}

// -------------------- Fix up tree (after loop) --------------------
void SimonTree::fixTree(shared_ptr<Node> root, int n) {
    shared_ptr<Node> node = root;
    while (!node->children.empty()) {
        node->start = 1;
        node = node->children.front();
    }
    node->start = 1;

    // Remove $ leaf
    node = root;
    while (!node->children.empty()) {
        node->end = n;
        node = node->children.back();
    }
    node->end = n;

    if (!root->children.empty() && root->children.back()->start == n + 1) {
        root->children.pop_back();
    }
}

// -------------------- Print Tree --------------------
void SimonTree::printTree(const shared_ptr<Node> &node, const string &w, string indent, bool isLast) {
    if (node->start != -1 && node->start <= (int)w.size()) {
        cout << indent;
        if (node->depth > 0) cout << (isLast ? "└── " : "├── ");
        string label = w.substr(node->start - 1, node->end - node->start + 1);
        string position = (node->start == node->end)
            ? "[" + to_string(node->start) + "]"
            : "[" + to_string(node->start) + ":" + to_string(node->end) + "] (k=" + to_string(node->depth) + ")";
        cout << label << position << "\n";
        indent += (isLast ? "    " : "│   ");
    }
    for (size_t i = 0; i < node->children.size(); ++i) {
        printTree(node->children[i], w, indent, i == node->children.size() - 1);
    }
}
