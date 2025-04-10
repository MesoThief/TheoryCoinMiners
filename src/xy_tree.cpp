#include <iostream>
#include <fstream>
#include <sstream>

#include "data/Trees.h"
#include "utils/Alphabet.h"

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
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "You must enter a test input file" << endl;
        cerr << "Usage: " << argv[0] << " <test-input-file-name>" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error opening " << inputFileName << endl;
        return 1;
    }

    // read inputs
    string alphabet;
    string t;
    string p;
    int k;
    
    getline(inputFile, alphabet);
    Alphabet::getInstance().setAlphabet(alphabet);

    getline(inputFile, t);
    
    getline(inputFile, p);
    
    string line;
    getline(inputFile, line);
    istringstream k_stream(line);
    k_stream >> k;
    
    RankerTable rankers = RankerTable(t);
    rankers.buildXRankerTable();
    rankers.buildYRankerTable();

    ShortlexResult pattern_shortlex = computePartialShortlexNormalForm(
        p,
        vector<int>(Alphabet::getInstance().size(), 1),
        vector<int>(Alphabet::getInstance().size() ,1),
        k + 1
    );

    shared_ptr<Node> T_X = Trees::buildXTree(rankers, pattern_shortlex, t);
    shared_ptr<Node> T_Y = Trees::buildYTree(rankers, pattern_shortlex, t);

    printTree(T_X, t);
    printTree(T_Y, t);

    return 0;
}
