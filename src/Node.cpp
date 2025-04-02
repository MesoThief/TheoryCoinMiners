// Node.cpp
#include "Node.h"

Node::Node(int s, int e, int d, std::shared_ptr<Node> p)
    : start(s), end(e), depth(d), parent(p) {}

bool Node::isRoot() const {
    return parent == nullptr;
}

bool Node::isLeaf() const {
    return children.empty();
}
