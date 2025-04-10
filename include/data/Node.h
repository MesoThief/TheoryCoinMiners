// Node.h
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>

struct Node {
    int start;
    int end;
    int depth;
    int r = 0;
    std::shared_ptr<Node> parent = nullptr;
    std::vector<std::shared_ptr<Node>> children;

    Node(int start, int end, int depth);
    Node(int start, int end, int depth, std::shared_ptr<Node> parent);

    
    bool isRoot() const;
    bool isLeaf() const;
};

#endif // NODE_H
