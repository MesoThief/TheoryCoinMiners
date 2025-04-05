#include <iostream>
#include <string>

#include "data/Trees.h"

using namespace std;

// -------------------- Print Tree --------------------
void printTree(const shared_ptr<Node>& node, const string& w, string indent = "", bool isLast = true, int depth = 0) {
    if (true) {
        cout << indent;
        if (depth > 0) cout << (isLast ? "└── " : "├── ");
        int position = node->start;
        cout << position << "\n";
        indent += (isLast ? "    " : "│   ");
    }
    for (size_t i = 0; i < node->children.size(); ++i) {
        printTree(node->children[i], w, indent, i == node->children.size() - 1, depth+1);
    }
}

// ------------------
// A simple test driver for tree
// ------------------
int main() {
    string t;
    cout << "Enter the value of text t: ";
    cin >> t;
    
    int as;
    cout << "Enter the value of alphabet size: ";
    cin >> as;
    
    string p;
    int universality;
    int k;
    cout << "Enter the value of pattern p: ";
    cin >> p;
    cout << "Enter the value of k: ";
    cin >> k;
    
    RankerTable rankers = RankerTable(t, as);
    rankers.buildXRankerTable();
    rankers.buildYRankerTable();

    ShortlexResult shortlex = computePartialShortlexNormalForm(
        p,
        vector<int>(26, 1),
        vector<int>(26 ,1),
        k
    );

    shared_ptr<Node> T_X = Trees::buildXTree(rankers, shortlex, t);

    printTree(T_X, t);

    return 0;
}
