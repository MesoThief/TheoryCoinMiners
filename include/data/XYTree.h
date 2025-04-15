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
        shared_ptr<Node> next = nullptr;    // TODO: Y-tree에서는 필요 없는 값. 최적화 시 Y-tree에선 삭제 가능.
    
        Node(int index);
    };

    struct Tree {
        shared_ptr<Node> root;

        vector<shared_ptr<Node>> parent;    // TODO: X-tree에서는 필요 없는 값. 최적화 시 X-tree에선 삭제 가능.
        vector<vector<int>> children;       // 논문에서는 interval이긴 한데, 일단 vector로 처리함
    };

    // Build X-tree using the X-ranker, ShortlexResult, and input text
    Tree buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    Tree buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text);
}
#endif // XYTREE_H
