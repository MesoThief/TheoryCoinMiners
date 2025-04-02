// Node.h
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>

struct Node {
    Node(std::shared_ptr<Node> sharedPtr);

    int start, end;
    int depth;
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    Node(int s, int e, int d, std::shared_ptr<Node> p = nullptr);

    bool isRoot() const;
    bool isLeaf() const;
};

#endif // NODE_H
