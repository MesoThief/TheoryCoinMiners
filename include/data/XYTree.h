#ifndef XYTREE_H
#define XYTREE_H

#include <string>
#include <memory>
#include "data/Ranker.h"
#include "data/Shortlex.h"
#include "utils/Common.h"

using namespace std;

namespace XYTree {
    struct Node {
        int index;
        int r = 0;
        shared_ptr<Node> next = nullptr;    // TODO: Y-tree에서는 필요 없는 값. 최적화 시 Y-tree에선 삭제 가능.
        Interval children = Interval(0, 0); // [start, end)
    
        Node(int index);
    };

    struct Tree {
        shared_ptr<Node> root;

        vector<shared_ptr<Node>> parent;    // TODO: X-tree에서는 필요 없는 값. 최적화 시 X-tree에선 삭제 가능.
    };

    // Build X-tree using the X-ranker, ShortlexResult, and input text
    Tree buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    Tree buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);
}
#endif // XYTREE_H
