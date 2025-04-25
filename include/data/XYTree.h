#ifndef XYTREE_H
#define XYTREE_H

#include <iostream>
#include <memory>
#include <string>

#include "data/Ranker.h"
#include "data/Shortlex.h"
#include "utils/Common.h"

using namespace std;

namespace XYTree {
    struct Node {
        int index;
        int r;
        shared_ptr<Node> next;  // TODO: Y-tree에서는 필요 없는 값. 최적화 시 Y-tree에선 삭제 가능.
        Interval children;      // [start, end]
        vector<int> arch_index; // For preprocessing checkpoint algorithm

        Node(int index);
    };

    inline std::ostream& operator<<(std::ostream& os, const Node& node) {
        if (node.index == INF)
            os << "(Node INF)";
        else
            os << "(Node " << node.index << ")";
        return os;
    }

    struct Tree {
        shared_ptr<Node> root;

        vector<shared_ptr<Node>> parent;  // TODO: X-tree에서는 필요 없는 값. 최적화 시 X-tree에선 삭제 가능.
    };

    // Build X-tree using the X-ranker, ShortlexResult, and input text
    Tree buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    Tree buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);
}  // namespace XYTree

#endif  // XYTREE_H
