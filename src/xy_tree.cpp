#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "data/XYTree.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"


using namespace std;
using namespace XYTree;

// -------------------- Print Tree --------------------
void printTree(const Tree& tree) {
    cout << "Visual Tree Structure:\n";
    cout << "----------------------\n";
    cout << left << setw(10) << "Index" << setw(10) << "Parent"
         << "Children\n";

    for (shared_ptr<XYTree::Node> current_node = tree.root->next; current_node != tree.root;
        current_node = current_node->next) {
        int i = current_node->index;
        cout << left << setw(10) << i;

        if (tree.parent[i]->index == INF) {
            cout << setw(10) << "INF";
        } else {
            cout << setw(10) << tree.parent[i]->index;
        }

        cout << "[" << current_node->children.start << "," << current_node->children.end << "]";

        cout << endl;
    }

    cout << "----------------------\n";
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

    cout << "t: " << t << endl;
    cout << "p: " << p << endl;

    RankerTable rankers = RankerTable(t);
    rankers.buildXRankerTable();
    rankers.buildYRankerTable();

    ShortlexResult pattern_shortlex = computePartialShortlexNormalForm(
        p, vector<int>(Alphabet::getInstance().size(), 1), vector<int>(Alphabet::getInstance().size(), 1), k + 1);

    XYTree::Tree T_X = buildXTree(rankers, pattern_shortlex, t);
    XYTree::Tree T_Y = buildYTree(rankers, pattern_shortlex, t);

    cout << "X-tree:" << endl;
    printTree(T_X);

    cout << endl;

    cout << "Y-tree:" << endl;
    printTree(T_Y);

    return 0;
}
