#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <limits>

using namespace std;

const int INF = numeric_limits<int>::max();

// -------------------- Node Definition --------------------
struct Node {
    int start, end;
    int depth;
    shared_ptr<Node> parent;
    vector<shared_ptr<Node>> children;

    Node(int s, int e, int d, shared_ptr<Node> p = nullptr)
        : start(s), end(e), depth(d), parent(p) {}

    bool isRoot() const {
        return parent == nullptr;
    }

    bool isLeaf() const {
        return children.empty();
    }
};

// -------------------- Compute X[i] --------------------
vector<int> computeX(const string& w) {
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
shared_ptr<Node> findNode(int i, const vector<int>& X, shared_ptr<Node> a) {
    while (!a->isRoot()) {
        int r = a->end;
        int rp = a->parent->end;
        if (X[i] >= r && X[i] < rp) {
            return a;
        } else {
            a->start = i + 1; // Close block
            a = a->parent;
        }
    }
    return a;
}

// -------------------- Split Node (Algorithm 3) --------------------
pair<shared_ptr<Node>, shared_ptr<Node>> splitNode(int i, shared_ptr<Node> root, shared_ptr<Node> a) {
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
void fixTree(shared_ptr<Node> root, int n) {
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
void printTree(const shared_ptr<Node>& node, const string& w, string indent = "", bool isLast = true) {
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

// -------------------- Main --------------------
int main() {
    string w;
    cout << "Enter the word: ";
    cin >> w;

    int n = w.size();
    string wp = w + "$";
    vector<int> X = computeX(w);

    auto root = make_shared<Node>(-1, n + 1, 0);
    shared_ptr<Node> p = root;

    for (int i = n; i >= 1; --i) {
        auto a = findNode(i, X, p);
        tie(root, p) = splitNode(i, root, a);
    }

    fixTree(root, n);

    cout << "\n Constructed Simon-Tree:\n\n";
    printTree(root, w);

    return 0;
}
