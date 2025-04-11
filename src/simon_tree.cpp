#include <iostream>

#include "data/SimonTree.h"

using namespace SimonTree;

// ------------------
// A simple test driver for SimonTree.cpp
// ------------------
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