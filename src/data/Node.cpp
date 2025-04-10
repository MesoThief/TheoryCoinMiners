#include "data/Node.h"

Node::Node(int start,int end,int depth)
    : start(start), end(end), depth(depth) {}

Node::Node(int start,int end,int depth, std::shared_ptr<Node> parent)
    : start(start), end(end), depth(depth), parent(parent) {}

bool Node::isRoot() const {
    return parent == nullptr;
}

bool Node::isLeaf() const {
    return children.empty();
}
