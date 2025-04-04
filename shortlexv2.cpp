#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding X- and Y-vectors.
struct ShortlexResult {
    string normalForm;
    vector<int> X; // X-coordinates for each letter in normalForm
    vector<int> Y; // Y-coordinates for each letter in normalForm
};

ShortlexResult computeShortlexFromVectors(const string& w, const vector<int>& X, const vector<int>& Y, int k) {
    int n = static_cast<int>(w.size());
    string v = w; // make a modifiable copy

    // --- Phase 2: Lexicographically reorder blocks with same (X, Y) and X+Y == k+1 ---
    int start = 0;
    while (start < n) {
        int end = start + 1;
        while (end < n &&
               X[end] == X[start] &&
               Y[end] == Y[start] &&
               (X[start] + Y[start] == k + 1)) {
            end++;
        }
        if (end - start > 1) {
            sort(v.begin() + start, v.begin() + end);
        }
        start = end;
    }

    ShortlexResult result;
    result.normalForm = v;
    result.X = X;
    result.Y = Y;
    return result;
}

unordered_map<char, stack<int>> buildStackForm(const string& shortlex) {
    unordered_map<char, stack<int>> sp;
    for (int i = shortlex.size() - 1; i >= 0; --i) {
        sp[shortlex[i]].push(i);
    }
    return sp;
}

unordered_map<string, int> rxMemo;

int RX(const string& w, int i, const string& x) {
    string key = to_string(i) + "#" + x;
    if (rxMemo.count(key)) return rxMemo[key];

    for (int j = i + 1; j <= (int)w.size(); ++j) {
        if (x < w.substr(i, j - i)) {
            return rxMemo[key] = j;
        }
    }
    return rxMemo[key] = w.size(); // default
}

struct XTree {
    struct Node {
        int id = -1;
        int parent = -1;
        int l = -1, r = -1;
        unordered_map<char, int> children;
    };

    vector<Node> nodes;
    unordered_map<int, int> prnt; // prnt[i] = parent of i
    unordered_map<int, pair<int, int>> chld; // chld[parent] = [start,end)

    XTree() {
        nodes.push_back(Node{0, -1, -1, -1}); // root node
    }

    void addNode(int id) {
        if ((int)nodes.size() <= id) nodes.resize(id + 1);
        if (nodes[id].id == -1) nodes[id].id = id;
    }

    void setChild(int parent, int i) {
        chld[parent].second = i + 1;
        if (chld[parent].first == -1) chld[parent].first = i;
    }

    void setParent(int i, int parent) {
        prnt[i] = parent;
    }
};

XTree constructXTree(const string& P, const string& shortlexP, unordered_map<char, stack<int>> sp) {
    XTree tree;
    int l = shortlexP.length();

    for (int j = 0; j < l; ++j) {
        char c = shortlexP[j];
        sp[c].pop(); // remove top from stack
    }

    for (int i = 0; i < (int)P.size(); ++i) {
        int parent = -1;
        for (char sigma : set<char>(P.begin(), P.end())) {
            parent = max(parent, RX(P, i, string(1, sigma)));
        }

        tree.addNode(parent);
        tree.addNode(i);
        tree.setParent(i, parent);

        stack<char> spCopy;
        for (auto& [ch, stk] : sp) {
            if (!stk.empty()) spCopy.push(ch);
        }

        while (!spCopy.empty()) {
            char top = spCopy.top(); spCopy.pop();
            tree.setChild(parent, i);
        }
    }

    return tree;
}

// ======= MAIN =======
int main() {
    string P = "bacbaabada";
    int k = 3;

    // Step 1: Define Σ and char-to-index map
    set<char> alphabetSet(P.begin(), P.end());
    vector<char> alphabet(alphabetSet.begin(), alphabetSet.end());
    unordered_map<char, int> charToIndex;
    for (int i = 0; i < alphabet.size(); ++i) {
        charToIndex[alphabet[i]] = i;
    }

    // Step 2: Input Σ-indexed X and Y vectors (manually or precomputed)
    int n = P.size();
    vector<vector<int>> Xvec = {
        {3,1,1,1}, {1,1,1,1}, {1,1,2,1}, {1,2,2,1}, {2,2,2,1},
        {3,2,2,1}, {3,3,2,1}, {4,3,2,1}, {4,4,2,2}, {4,4,2,2}
    };
    vector<vector<int>> Yvec = {
        {3,3,3,3}, {3,3,3,3}, {3,3,2,3}, {2,2,2,3}, {2,2,2,3},
        {2,2,2,3}, {1,1,2,3}, {1,1,2,3}, {1,1,2,2}, {1,1,1,2}
    };

    // Step 3: Convert to scalar X and Y
    vector<int> X(n), Y(n);
    for (int i = 0; i < n; ++i) {
        char c = P[i];
        int idx = charToIndex[c];
        X[i] = Xvec[i][idx];
        Y[i] = Yvec[i][idx];
    }

    // Step 4: Compute ShortLex from (X,Y) vectors
    auto result = computeShortlexFromVectors(P, X, Y, k);
    string shortlexP = result.normalForm;

    cout << "ShortLex(P): " << shortlexP << endl;

    // Step 5: Build stack form from ShortLex
    auto sp = buildStackForm(shortlexP);

    // Step 6: Construct X-tree from pattern and stack
    auto tree = constructXTree(P, shortlexP, sp);

    // Step 7: Print X-tree
    cout << "X-Tree Parent Map:\n";
    for (const auto& [child, parent] : tree.prnt) {
        cout << "Node " << child << " → Parent " << parent << "\n";
    }

    return 0;
}