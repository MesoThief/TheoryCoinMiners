#ifndef XYTREE_H
#define XYTREE_H

#include <string>
#include <memory>
#include "Ranker.h"
#include "Shortlex.h"

using namespace std;

namespace XYTree {
    struct Node {
        int index;
        int r;
        shared_ptr<Node> parent = nullptr;
        vector<shared_ptr<Node>> children;
        shared_ptr<Node> next = nullptr;
    
        Node(int index);
    };

    // Build X-tree using the X-ranker, ShortlexResult, and input text
    shared_ptr<Node> buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    shared_ptr<Node> buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);
}
#endif // XYTREE_H
